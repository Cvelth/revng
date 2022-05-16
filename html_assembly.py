#
# This file is distributed under the MIT License. See LICENSE.md for details.
#

import argparse
import os
import shutil
import subprocess
import sys
from threading import Thread

import yaml


def accept_arguments():
    parser = argparse.ArgumentParser(
        description=(
            "Provides an easy way to produce "
            "revng disassembled view for a binary.\n"
            "It relies on revng pipelines under the hood."
        )
    )

    parser.add_argument(
        "path_to_the_binary",
        metavar="<path to the binary>",
        type=str,
        help="Specifies the directory where the input binary should be loaded from",
    )
    parser.add_argument(
        "output_directory",
        metavar="<output directory>",
        type=str,
        default=".",
        help="Specifies the directory where to output the results to",
    )
    parser.add_argument(
        "-r",
        "--path-to-revng",
        type=str,
        default=None,
        help="Specifies a custom path to the revng script",
        metavar="<path>",
        dest="path_to_revng",
    )
    parser.add_argument(
        "--save-pipeline-intermediates",
        action="store_true",
        help="Determines whether a path should be passed to the `-p` option of `revng pipeline`",
        dest="save_pipeline_intermediates",
    )
    parser.add_argument(
        "--keep-temporaries",
        action="store_true",
        help="Prevents temporary directories from being deleted after the script is over",
        dest="keep_temporaries",
    )

    return parser.parse_args()


def invoke_pipeline(
    command: str,
    input_step: str,
    input_path: str,
    output_step: str,
    output_path: str,
    input_container: str,
    output_container: str,
    container_type: str,
    model_path: str = None,
    save_model: str = None,
    temporary_path: str = None,
):
    os.makedirs(os.path.dirname(output_path), exist_ok=True)

    full_command = [
        command,
        "pipeline",
        "--verbose",
        output_container + ":*:" + container_type,
        "-i",
        input_step + ":" + input_container + ":" + input_path,
        "-o",
        output_step + ":" + output_container + ":" + output_path,
        "--step",
        output_step,
    ]

    if temporary_path is not None:
        full_command.append("-p")
        full_command.append(temporary_path)
    if model_path is not None:
        full_command.append("-m")
        full_command.append(model_path)
    if save_model is not None:
        full_command.append("-save-model")
        full_command.append(save_model)

    process = subprocess.run(full_command)
    assert process.returncode == 0


def until_internal(command: str, input_file: str, output_directory: str, working_directory):
    output_file = output_directory + "/temporary/internal.yml.yml"
    model_path = output_directory + "/temporary/model.yml"
    invoke_pipeline(
        command,
        input_step="begin",
        input_path=input_file,
        output_step="ProcessAssembly",
        output_path=output_file,
        input_container="input",
        output_container="assembly-internal.yml",
        container_type="FunctionAssemblyInternal",
        save_model=model_path,
        temporary_path=working_directory,
    )

    return (output_file, model_path)


def export_html(name: str, contents: str, path: str, style: str):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as file:
        file.write('<!DOCTYPE html><html lang="en-US"><head>')
        file.write("<title>")
        file.write(name)
        file.write("</title>")
        file.write('<meta charset="utf-8">')
        file.write('<link rel="icon" href="https://rev.ng/favicon.ico">')
        file.write('<link href="' + style + 'style/assembly.css" rel="stylesheet">')
        file.write('<script src="../script/button.js" defer></script>')
        file.write("</head><body>")
        file.write(contents)
        file.write("</body></html>")


def export_svg(name: str, contents: str, path: str):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as file:
        file.write('<!DOCTYPE html><html lang="en-US"><head>')
        file.write("<title>")
        file.write(name)
        file.write("</title>")
        file.write('<meta charset="utf-8">')
        file.write('<link rel="icon" href="https://rev.ng/favicon.ico">')
        file.write('<link href="../style/assembly.css" rel="stylesheet">')
        file.write('<link href="../style/cfg.css" rel="stylesheet">')
        file.write('<script src="../script/button.js" defer></script>')
        file.write("</head><body>")
        file.write(contents)
        file.write("</body></html>")


def fix_url(string: str):
    forbidden_charactes = {" ", ":", "!", "#", "?", "<", ">", "/", "\\", "{", "}", "[", "]"}

    output = ""
    for char in string:
        if char in forbidden_charactes:
            output = output + "_"
        else:
            output = output + char
    return output


def export_assembly(
    command: str, input_file: str, model_path: str, output_directory: str, working_directory
):
    assembly_path = output_directory + "/temporary/assembly.html.yml"
    invoke_pipeline(
        command,
        input_step="ProcessAssembly",
        input_path=input_file,
        output_step="YieldAssembly",
        output_path=assembly_path,
        input_container="assembly-internal.yml",
        output_container="assembly-html.yml",
        container_type="FunctionAssemblyHTML",
        model_path=model_path,
        temporary_path=working_directory,
    )

    with open(assembly_path, "r") as file:
        for address, html in yaml.safe_load(file).items():
            export_html(
                fix_url(address),
                html,
                output_directory + "/assembly/" + fix_url(address) + ".html",
                "../",
            )


def export_cfg(
    command: str, input_file: str, model_path: str, output_directory: str, working_directory
):
    cfg_path = output_directory + "/temporary/cfg.svg.yml"
    invoke_pipeline(
        command,
        input_step="ProcessAssembly",
        input_path=input_file,
        output_step="YieldCFG",
        output_path=cfg_path,
        input_container="assembly-internal.yml",
        output_container="cfg-svg.yml",
        container_type="FunctionControlFlowGraphSVG",
        model_path=model_path,
        temporary_path=working_directory,
    )

    with open(cfg_path, "r") as file:
        for address, svg in yaml.safe_load(file).items():
            export_svg(
                fix_url(address),
                svg,
                output_directory + "/cfg/" + fix_url(address) + ".html",
            )


def export_index(internal_dictionary, path: str):
    contents = ""

    address_name_pairs = {}
    for address, internal in internal_dictionary.items():
        parsed = yaml.safe_load(internal)
        address_name_pairs[address] = parsed["Name"]

    for address, name in address_name_pairs.items():
        contents = (
            contents
            + '<div class="instruction">'
            + '<span class="mnemonic">'
            + name
            + "</span>: "
            + '<span class="function-label">'
            + '<a class="function-link" href="assembly/'
            + fix_url(address)
            + ".html#basic_block_at_"
            + fix_url(address)
            + '">assembly</a></span>, '
            + '<span class="function-label">'
            + '<a class="function-link" href="cfg/'
            + fix_url(address)
            + ".html#basic_block_at_"
            + fix_url(address)
            + '">cfg</a></span>'
            + "</div>"
        )

    export_html("index", contents, path, "")


def export_extra(path: str):
    os.makedirs(path + "/style", exist_ok=True)
    with open(path + "/style/assembly.css", "w") as file:
        file.write(
            """
@import url('https://fonts.googleapis.com/css2?family=Source+Code+Pro&display=swap');
body {
  background-color: #2e2f30;
  font-family: 'Source Code Pro', monospace;
  font-size: 18px;
  line-height: 1.25;
  white-space: nowrap;
}

a {
  text-decoration: none;
  color: inherit;
}

:target {
  background-color: #252627;
}

.basic-block {
  padding: 12px;
}

.instruction {
  padding: 2px;
  padding-left: 2ch;
  color: #d6cf9a;
}

.untagged {
  color: #d6cf9a;
}

.basic-block-label,
.function-label {
  color: #d6c540;
}

.basic-block-owner {
  display: none;
}

.instruction-target {
  color: #a69a3f;
}

.instruction-opcode,
.mnemonic,
.mnemonic-prefix,
.mnemonic-suffix {
  color: #ff8080;
}

.immediate-value {
  color: #a87e49;
}

.memory-operand {
  color: #d6cf9a;
}

.register {
  color: #66a334;
}

.comment {
  color: #a8abb0;
  font-size: 15px;
}

.instruction-bytes,
.instruction-address {
  font-size: 12px;
  color: #71747f;
}

.instruction-bytes {
  display: none;
}

.error {
  color: #e05050;
  font-size: 15px;
}

.instruction-opcode {
  font-size: 0px;
  display: none;
}
            """
        )

    os.makedirs(path + "/style", exist_ok=True)
    with open(path + "/style/cfg.css", "w") as file:
        file.write(
            """
.node-body {
  display: none;
}

.node-contents {
  border: 3px solid #d6cf9a;
  border-radius: 5px;
  background:transparent;
}

.unconditional-edge {
  stroke: #d6cf9a;
  stroke-width: 2px;
}
.taken-edge {
  stroke: #33aa33;
  stroke-width: 2px;
}
.not-taken-edge {
  stroke: #aa3333;
  stroke-width: 2px;
}

#unconditional-arrow-head {
  fill: #d6cf9a;
}
#taken-edge-arrow-head {
  fill: #33aa33;
}
#not-taken-arrow-head {
  fill: #aa3333;
}
            """
        )

    os.makedirs(path + "/script", exist_ok=True)
    with open(path + "/script/button.js", "w") as file:
        file.write(
            """
var button = document.createElement("Button");
button.style.zIndex = 999;
button.style.border = "none";
button.style.bottom = 0;
button.style.right = 0;
button.style.position = "fixed";
button.style.width = "75px";
button.style.height = "75px";
button.style.backgroundColor = "transparent";
button.style.backgroundImage = "url(../resource/button.png)";
button.style.backgroundSize = "cover";

button.onclick = function() {
  let position = window.location.href.search('assembly')
  if (position != -1) {
    window.location = window.location.href.replace('assembly', 'cfg')
  } else {
    let position = window.location.href.search('cfg')
    if (position != -1) {
      window.location = window.location.href.replace('cfg', 'assembly')
    } else {
      reportError("unsupported url");
    }
  }
}

document.body.appendChild(button);
            """
        )


def main():
    arguments = accept_arguments()

    extra_thread = Thread(target=export_extra, args=(arguments.output_directory,))
    extra_thread.start()

    command = "revng"
    if arguments.path_to_revng is not None:
        command = arguments.path_to_revng + "/revng"

    working_directory = None
    if arguments.save_pipeline_intermediates:
        working_directory = arguments.output_directory + "/temporary/pipeline/"

    internal_path, model_path = until_internal(
        command, arguments.path_to_the_binary, arguments.output_directory, working_directory
    )

    assembly_thread = Thread(
        target=export_assembly,
        args=(command, internal_path, model_path, arguments.output_directory, working_directory),
    )
    assembly_thread.start()

    # cfg_thread = Thread(
    #     target=export_cfg,
    #     args=(command, internal_path, model_path, arguments.output_directory, working_directory),
    # )
    # cfg_thread.start()

    with open(internal_path, "r") as file:
        export_index(
            yaml.safe_load(file),
            arguments.output_directory + "/index.html",
        )

    extra_thread.join()
    assembly_thread.join()
    # cfg_thread.join()

    if not arguments.keep_temporaries:
        shutil.rmtree(arguments.output_directory + "/temporary")


if __name__ == "__main__":
    sys.exit(main())
