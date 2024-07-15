/*
 * This file is distributed under the MIT License. See LICENSE.md for details.
 */

import "bootstrap";
import "bootstrap/dist/css/bootstrap.min.css";
import "datatables.net-dt/css/dataTables.dataTables.min.css";
import "./main.css";

import sqlite3InitModule, { Database, SQLite3Error, Sqlite3Static } from "@sqlite.org/sqlite-wasm";
import DataTable, { Api, ConfigColumns } from "datatables.net-dt";
import * as ApexCharts from "apexcharts";
import { Tarball } from "@obsidize/tar-browserify";
import { saveAs } from "file-saver";
import * as yaml from "yaml";
import { basename } from "path";
import { filesize } from "filesize";

// We inject some variables into `window`, for easier debugging
declare global {
    interface Window {
        _sqlite3?: Sqlite3Static;
        db?: Database;
    }
}

// Interface describing the contents of `meta.yml`
interface Metadata {
    extra_columns?: {
        name: string;
        label: string;
        renderer?: string;
        align?: "left" | "right";
    }[];

    downloads?: {
        name: string;
        label: string;
    }[];

    ordering?: {
        name: string;
        dir: "asc" | "desc";
    }[];

    notes?: string;
    reproducer_prelude?: string;

    // Autogenerated by `revng mass-testing run`
    cpu_count: number;
    start_time: number;
}

async function initSqlite3(): Promise<Sqlite3Static> {
    if (window._sqlite3 !== undefined) {
        return window._sqlite3;
    } else {
        const sqlite3 = await sqlite3InitModule({
            print: console.log,
            printErr: console.error,
        });
        window._sqlite3 = sqlite3;
        return sqlite3;
    }
}

// Truncates a number to the desired decimal digits
function truncate(num: number, digits: number): number {
    return Math.trunc(num * 10 ** digits) / 10 ** digits;
}

// Given num and total, return a string representing the percent between them
function percent(num: any, total: any): string {
    return `${truncate((num * 100) / total, 2)}%`;
}

// Helper function that creates an element and adds it as a child to a parent
function createAndAppend<T = HTMLElement>(elem: Element, type: string): T {
    return elem.appendChild(document.createElement(type)) as T;
}

// Given a URL, load it as an sqlite database and return it
async function loadDBFromURL(url: string | URL): Promise<Database> {
    const sqlite3 = await initSqlite3();
    const req = await fetch(url);
    const buffer = await req.arrayBuffer();
    const db_buffer = sqlite3.wasm.allocFromTypedArray(buffer);
    const db = new sqlite3.oo1.DB();
    const rc = sqlite3.capi.sqlite3_deserialize(
        db,
        "main",
        db_buffer,
        buffer.byteLength,
        buffer.byteLength,
        sqlite3.capi.SQLITE_DESERIALIZE_FREEONCLOSE
    );
    db.checkRc(rc);
    return db;
}

export function sleep(time: number): Promise<void> {
    return new Promise((res, rej) => setTimeout(() => res(undefined), time));
}

// Given a name and a URL to the trace file, open in a new tab the trace in
// ui.perfetto.dev
async function openPerfetto(name: string, trace: string): Promise<void> {
    // There's a limited time between the click event (start of this function)
    // and the time where the browser will block the window.open as a popup. In
    // order to alleviate this:
    // 1. We start requesting the trace via fetch, but don't await it yet
    //    (since the function is async it will return immediately)
    // 2. We call window.open, which most likely will fall in the window
    //    between a click event and the browser showing a popup blocker, so it
    //    will go through smoothly
    // 3. Both the trace file and the interface will have to transfer some data
    //    which synergizes nicely
    // 4. Once the trace is loaded, we pass it to the tab via `postMessage`
    const req_promise = fetch(trace);
    const handle = window.open("https://ui.perfetto.dev", "_blank");
    if (handle === null) {
        return;
    }
    let ponged = false;
    // Wait for the tab to have fully loaded, this will be signaled once the
    // tab will return a `PING` in response to our `PONG` message
    window.addEventListener("message", function listener(ev) {
        if (ev.data === "PONG") {
            ponged = true;
            window.removeEventListener("message", listener);
        }
    });
    for (;;) {
        handle.postMessage("PING", "*");
        await sleep(100);
        if (ponged) {
            break;
        }
    }
    // The tab is loaded, now we actually wait for the trace file to complete
    // transferring. If we're lucky since a bit of time has passed the `await`
    // might return straight away
    const req = await req_promise;
    if (!req.ok) {
        return;
    }
    // Actually send the trace to the tab via `postMessage`
    handle.postMessage(
        {
            perfetto: {
                buffer: await req.arrayBuffer(),
                title: `Trace of ${name}`,
                fileName: basename(trace),
            },
        },
        "*"
    );
}

// This will return a reproducer tar to re-create a crash.
async function createReproducer(name: string, meta: Metadata): Promise<Uint8Array | undefined> {
    const binReq = await fetch(`${name}/input`);
    if (!binReq.ok) {
        return undefined;
    }
    const tarball = new Tarball();
    tarball.addBinaryFile("input", new Uint8Array(await binReq.arrayBuffer()), { fileMode: 0o444 });

    const commandReq = await fetch(`${name}/test-harness.json`);
    let command: string[] = (await commandReq.json()).command;

    for (let i = 0; i < command.length; i++) {
        if (command[i] == "%INPUT%") {
            command[i] = "input";
        }
    }

    const script = `#!/usr/bin/env bash
set -euo pipefail
SCRIPT_DIR=$(realpath "$(dirname "\${BASH_SOURCE[0]}")")

${meta.reproducer_prelude || ""}

${command.join(" ")} "$@"
`;
    tarball.addTextFile("go.sh", script, { fileMode: 0o555 });

    return tarball.toUint8Array();
}

// Nav items, each key can have multiple strings, the first one will be used
// to generate the `<a>` element pointing to it
const NAV_ITEMS: Record<string, string[]> = {
    Overview: ["index.html", ""],
    Failures: ["failures.html"],
    Crashes: ["crashes.html"],
    Timeouts: ["timeouts.html"],
    OOMs: ["ooms.html"],
    Successes: ["successes.html"],
    "Raw Data": ["raw_data.html"],
};

// Fills the specified element with a navbar to other pages, as specified in
// the NAV_ITEMS variable
function populateNav(element: HTMLElement) {
    const parts = [];
    for (const [title, page_names] of Object.entries(NAV_ITEMS)) {
        if (page_names.includes(basename(window.location.pathname))) {
            parts.push(`<span>${title}</span>`);
        } else {
            parts.push(`<a href="${page_names[0]}">${title}</a>`);
        }
    }

    element.innerHTML = parts.join("&nbsp;|&nbsp;");
}

// Given a number of seconds, format it in one of the following ways:
// showHours=false => MM:ss.mmm
// showHours=true  => hh:MM:ss.mmm
function formatTime(elapsedSeconds: number, showHours: boolean): string {
    const seconds = elapsedSeconds % 60;
    let minutes = 0;
    let hours = 0;
    if (showHours) {
        minutes = Math.floor((elapsedSeconds / 60) % 60);
        hours = Math.floor(elapsedSeconds / 3600);
    } else {
        minutes = Math.floor(elapsedSeconds / 60);
    }
    let res = `${truncate(seconds, 3)}`;
    if (minutes > 0) {
        res = `${minutes}:${res}`;
    }
    if (hours > 0) {
        res = `${hours}:${res}`;
    }
    return res;
}

// Given the input element, populate it with some overall stats about the
// current execution of mass testing, plus a pie chart for
// failures/crashes/ooms/timeouts/successes
function populateGeneralStats(element: HTMLElement, db: Database, meta: Metadata) {
    const pre = createAndAppend(element, "pre");
    const pieDiv = createAndAppend(element, "div");
    element.classList.add("stats");

    // Use the SUM/IIF combo to get aggregated values about the current run
    const sqlString = `
        SELECT
            COUNT(*) as total,
            SUM(IIF(main.status = 'OK', 1, 0)) as ok,
            SUM(IIF(main.status = 'FAILED', 1, 0)) as failed,
            SUM(IIF(main.status = 'OOM', 1, 0)) as oomed,
            SUM(IIF(main.status = 'CRASHED', 1, 0)) as crashed,
            SUM(IIF(main.status = 'TIMED_OUT', 1, 0)) as timed_out,
            SUM(elapsed_time) as total_time
        FROM main`;
    const stats = db.exec({ sql: sqlString, rowMode: "object", returnValue: "resultRows" })[0];
    const data = [
        ["Successes", stats["ok"], "#00e396"],
        ["Failures", stats["failed"], "#e6c000"],
        ["Crashes", stats["crashed"], "#f93636"],
        ["Timed out", stats["timed_out"], "#fc923c"],
        ["OOMs", stats["oomed"], "#3c63fc"],
    ];
    pre.innerHTML = `Statistics:\nTotal run: ${stats["total"]}\n`;
    for (const [label, num, _] of data) {
        pre.innerHTML += `${label}: ${num} (${percent(num, stats["total"])})\n`;
    }

    const cpuCount = meta.cpu_count;
    const clockTime = (stats["total_time"] as number) / cpuCount;
    pre.innerHTML += `\nStart time: ${new Date(meta.start_time * 1000).toISOString()}\n`;
    pre.innerHTML += `Total runtime: ${formatTime(clockTime, true)} (CPU count: ${cpuCount})`;
    // If meta has the notes field, add it to the pre
    if (meta.notes !== undefined) {
        pre.innerHTML += `\n${meta.notes}\n`;
    }

    // Add the pie chart with the same information as the `pre` above
    pieDiv.className = "pieChart";
    const chart = new ApexCharts(pieDiv, {
        chart: {
            type: "pie",
            width: `${pieDiv.getClientRects()[0].width}px`,
            animations: {
                enabled: false,
            },
        },
        plotOptions: {
            pie: {
                expandOnClick: false,
            },
        },
        legend: {
            show: false,
        },
        series: data.map((e) => e[1]),
        labels: data.map((e) => e[0]),
        colors: data.map((e) => e[2]),
    });
    chart.render();
}

// Given an element and a category (e.g. CRASHED, TIMED_OUT) gather the
// per-component statistics and display them, both in textual form and as a pie
// chart
function populateCrashStats(element: Element, db: Database, category: string) {
    element.classList.add("stats");
    const pre = createAndAppend(element, "pre");
    pre.style.marginRight = "auto";
    const pieDiv = createAndAppend(element, "div");

    const raw_counts = db.exec({
        sql: "SELECT name, count FROM crash_components WHERE category = ?",
        bind: [category],
        rowMode: "object",
        returnValue: "resultRows",
    });

    let total = 0;
    // Counts is an array of <component name, count>
    const counts: [string, number][] = [];
    for (const entry of raw_counts) {
        counts.push([entry.name as string, entry.count as number]);
        total += entry.count as number;
    }
    // Sort the results, so that the biggest offenders appear first
    counts.sort((a, b) => (a[1] === b[1] ? 0 : a[1] < b[1] ? 1 : -1));

    pre.innerHTML = "";
    for (const [name, count] of counts) {
        const count_percent = percent(count, total);
        pre.innerHTML += `${name}: ${count} (${count_percent})\n`;
    }

    pieDiv.className = "pieChart";
    const categoryChart = new ApexCharts(pieDiv, {
        chart: {
            type: "pie",
            width: `${pieDiv.getClientRects()[0].width}px`,
            animations: {
                enabled: false,
            },
        },
        plotOptions: {
            pie: {
                expandOnClick: false,
            },
        },
        legend: {
            show: false,
        },
        series: counts.map((e) => e[1]),
        labels: counts.map((e) => e[0]),
    });
    categoryChart.render();
}

// This defines the renderers, which are function that allow datatables to
// display data in a different way without converting it in-place (e.g. keeping
// bytes as a number while displaying it in human-readable form). This is
// defined in this way as it can be referenced in `meta.yml` in the renderer
// attribute of a column
const RENDERERS: Record<string, (data: any, type: string) => any> = {
    time: (data: number, type) => (type === "display" ? formatTime(data, false) : data),
    filesize: (data: number, type) => (type !== "display" ? data : filesize(data, { base: 2 })),
    ellipsis: (data: string, type) => {
        if (type !== "display") {
            return data;
        }
        return data.length < 70 ? data : data.slice(0, 70) + "…";
    },
};

interface Columns extends ConfigColumns {
    data?: string;
    render?: (data: any, type: string, row: any) => any;
    // special boolean that allows not using the render if the data is shown in
    // the detail page
    useRenderForDetail?: boolean;
}

// Given a Metadata instance, return an array of Columns that can be fed to
// datatables to display the data stored in `main.db`
function getColumns(meta: Metadata): Columns[] {
    const tableColumns: Columns[] = [
        {
            name: "name",
            data: "name",
            title: "name",
            width: "40%",
            render: RENDERERS["ellipsis"],
            useRenderForDetail: false,
        },
        {
            name: "elapsed_time",
            data: "elapsed_time",
            title: "time",
            render: RENDERERS["time"],
            className: "dt-body-right",
        },
        { name: "exit_code", data: "exit_code", title: "exit code", className: "dt-body-right" },
        { name: "status", data: "status", title: "status" },
        { name: "stacktrace_id", data: "stacktrace_id", title: "stacktrace id", orderable: false },
    ];

    for (const elem of meta.extra_columns || []) {
        const obj: Record<string, any> = { name: elem.name, data: elem.name, title: elem.label };
        if (elem.renderer !== undefined) {
            obj["render"] = RENDERERS[elem.renderer];
        }
        if (elem.align !== undefined) {
            obj["className"] = `dt-body-${elem.align}`;
        }
        tableColumns.push(obj);
    }

    tableColumns.push({
        title: "downloads",
        orderable: false,
        render: function (data, type, row) {
            const container = document.createElement("div");

            // Helper function that will return a button element with the
            // specified text. If the target is a string (link) an `<a>` will
            // added, whereas if it's a function an onclick will be added
            function button(text: string, target: string | (() => any)) {
                const button = document.createElement("button");
                button.innerHTML = text;
                button.className = "btn btn-primary btn-sm";
                if (typeof target === "string") {
                    const anchor = document.createElement("a");
                    anchor.href = target;
                    anchor.target = "_blank";
                    anchor.appendChild(button);
                    return anchor;
                } else {
                    button.onclick = target;
                    return button;
                }
            }

            container.appendChild(button("Bin", `${row.name}/input`));

            for (const elem of meta.downloads || []) {
                container.appendChild(button(elem.label, `${row.name}/${elem.name}`));
            }

            container.appendChild(button("Log", `${row.name}/output.log`));

            if (row.has_trace) {
                const trace = `${row.name}/trace.json.gz`;
                container.appendChild(button("Trace", trace));
                container.appendChild(
                    button("UPD", async () => await openPerfetto(row.name, trace))
                );
            }

            container.appendChild(
                button("Repro", async () => {
                    const tar = await createReproducer(row.name, meta);
                    if (tar !== undefined) {
                        saveAs(
                            new Blob([tar], { type: "application/tar" }),
                            `${basename(row.name)}-reproducer.tar`
                        );
                    }
                })
            );

            container.appendChild(button("Files", `${row.name}/`));
            if (!(row._inDetail ?? false)) {
                container.appendChild(button("Det", `binary.html#${row.name}`));
            }

            return container;
        },
    });

    return tableColumns;
}

// Class representing the search state, will be serialized/deserialized in the
// window's `hash` to allow persisting a specific search
class SearchState {
    // The query string
    public query: string;
    // Whether the query is "plan" or SQL
    public sql: boolean;

    constructor(query: string, sql: boolean) {
        this.query = query;
        this.sql = sql;
    }

    toString() {
        return btoa(JSON.stringify(this));
    }

    static fromString(data: string): SearchState {
        const obj = JSON.parse(atob(data));
        return new SearchState(obj.query, obj.sql);
    }
}

// This function will add a datatable to the target element with the data in
// the database's `main` table
function populateDatatable(element: Element, db: Database, meta: Metadata) {
    let queryString = "SELECT * FROM main WHERE 1=1";
    const filter = element.getAttribute("data-filter") ?? "";
    if (filter !== "") {
        queryString += ` AND ${filter}`;
    }

    function search(searchState: SearchState, datatable: Api<any>) {
        if (searchState.sql) {
            let data;
            try {
                data = getData(`${queryString} AND (${searchState.query})`);
            } catch (e) {
                if (e instanceof SQLite3Error) {
                    return;
                }
                throw e;
            }
            // Clear the data and add the new records
            datatable.clear();
            datatable.rows.add(data);
        } else {
            datatable.search(searchState.query);
        }
        // Re-draw the datatable
        datatable.draw();
    }

    const getData = (sql: string) => db.exec({ sql, rowMode: "object", returnValue: "resultRows" });
    function searchBar(settings: { api: Api<any> }): HTMLElement {
        const datatable = settings.api;
        const container = document.createElement("div");

        const sqlLabel = createAndAppend<HTMLLabelElement>(container, "label");
        sqlLabel.innerHTML = "SQL: ";
        sqlLabel.htmlFor = "dt-search-sql";

        const sqlInput = createAndAppend<HTMLInputElement>(container, "input");
        sqlInput.id = "dt-search-sql";
        sqlInput.type = "checkbox";
        sqlInput.style.margin = "0 1% 0 0.5%";

        const label = createAndAppend<HTMLLabelElement>(container, "label");
        label.innerHTML = "Search: ";
        label.htmlFor = "dt-search";

        const input = createAndAppend<HTMLInputElement>(container, "input");
        input.type = "search";
        input.id = "dt-search";
        input.style.width = "70%";
        input.style.marginLeft = "0.5%";

        function updateState() {
            const state = new SearchState(input.value, sqlInput.checked);
            window.location.hash = `#${state.toString()}`;
            return state;
        }

        // When this function is called, the window is loading. If we find an
        // hash it means there was a previous search. Load it back as the
        // search state
        if (window.location.hash.startsWith("#")) {
            const state = SearchState.fromString(window.location.hash.slice(1));
            sqlInput.checked = state.sql;
            input.value = state.query;
            search(state, datatable);
        }

        // When checking the SQL checkbox, reset the Datatables search, whereas
        // when unchecking it restore the data without the SQL filter
        sqlInput.addEventListener("input", () => {
            if (sqlInput.checked) {
                // Clear the search in case it was set set previously
                datatable.search("");
            } else {
                // Re-fetch the data from SQlite without filters
                const data = getData(queryString);
                datatable.clear();
                datatable.rows.add(data);
            }
            datatable.draw();
            updateState();
        });

        // Add event handlers to react to user input. If SQL mode is off then
        // the search is performed in real-time as the user types. When it is
        // on the search is only performed when pressing enter, otherwise the
        // page lags **a lot**
        input.addEventListener("input", () => {
            if (!sqlInput.checked) {
                search(updateState(), datatable);
            }
        });

        input.addEventListener("keyup", (ev) => {
            if (sqlInput.checked && (ev.key === "Enter" || ev.keyCode === 13)) {
                search(updateState(), datatable);
            }
        });

        return container;
    }

    // Actually create the table
    new DataTable(element, {
        data: getData(queryString),
        columns: getColumns(meta),
        order: meta.ordering,
        layout: {
            topStart: "pageLength",
            topEnd: searchBar as unknown as null,
            bottomStart: "info",
            bottomEnd: "paging",
        },
    });
}

// Given the input element, populate it with data about a single execution
function populateBinaryDetail(element: HTMLElement, db: Database, meta: Metadata) {
    const hash = window.location.hash;
    if (!hash.startsWith("#")) {
        return;
    }

    const name = hash.slice(1);
    document.getElementsByTagName("title")[0].innerHTML += ` ${name}`;
    const data: Record<string, any> = db.exec({
        sql: "SELECT * FROM main WHERE name = ?",
        bind: [name],
        rowMode: "object",
        returnValue: "resultRows",
    })[0];
    data["_inDetail"] = true;

    const title = createAndAppend(element, "h2");
    title.innerHTML = `Details for ${name}`;

    const table = createAndAppend(element, "table");
    table.className = "table table-bordered";
    table.style.width = "70%";

    const colgroup = createAndAppend(table, "colgroup");
    const keyCol = createAndAppend(colgroup, "col");
    keyCol.setAttribute("span", "1");
    keyCol.style.width = "30%";
    const valueCol = createAndAppend(colgroup, "col");
    valueCol.setAttribute("span", "1");

    const tbody = createAndAppend(table, "tbody");
    for (const column of getColumns(meta)) {
        const tr = createAndAppend(tbody, "tr");
        const tdKey = createAndAppend(tr, "td");
        const tdValue = createAndAppend(tr, "td");

        tdKey.innerHTML = column.title || column.name || "";
        if (column.render !== undefined && (column.useRenderForDetail ?? true)) {
            const cell = column.render(data[column.data as string], "display", data);
            if (cell instanceof HTMLElement) {
                tdValue.appendChild(cell);
            } else {
                tdValue.innerHTML = cell;
            }
        } else {
            tdValue.innerHTML = data[column.data as string];
        }
    }
}

// Entrypoint, will dispatch to the right function based on element ids or
// classes
async function main() {
    const db = await loadDBFromURL("main.db");
    window.db = db;

    const metaReq = await fetch("meta.yml");
    const meta: Metadata = yaml.parse(await metaReq.text());

    const navDiv = document.getElementById("nav");
    if (navDiv !== null) {
        populateNav(navDiv);
    }

    const statsDiv = document.getElementById("stats");
    if (statsDiv !== null) {
        populateGeneralStats(statsDiv, db, meta);
    }

    for (const element of Array.from(document.getElementsByClassName("crash-stats"))) {
        populateCrashStats(element, db, element.getAttribute("data-category")!);
    }

    const dataTable = document.getElementById("data-table");
    if (dataTable !== null) {
        populateDatatable(dataTable, db, meta);
    }

    const binaryDetail = document.getElementById("binary-detail");
    if (binaryDetail !== null) {
        populateBinaryDetail(binaryDetail, db, meta);
    }
}

main().then(() => {});
