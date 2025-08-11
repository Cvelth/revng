#!/usr/bin/env python3

#
# This file is distributed under the MIT License. See LICENSE.md for details.
#

import argparse
import os
import sys
from dataclasses import asdict, dataclass, field

import yaml


@dataclass
class Entry:
    addressless_counts: list[dict[str, float]] = field(default_factory=list)
    average_addressless: float = 0.0
    variety_counts: list[dict[str, float]] = field(default_factory=list)
    average_variety: float = 0.0


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument("input_directory")

    return parser.parse_args()


def main():
    args = parse_args()

    result: dict[str, Entry] = {}

    for file in os.listdir(args.input_directory + "/output/"):
        with open(args.input_directory + "/output/" + file, encoding="utf-8") as file_stream:
            output = yaml.safe_load(file_stream)

            for step in output["Steps"]:
                if step["Path"] not in result:
                    result[step["Path"]] = Entry()

                result[step["Path"]].addressless_counts.append({file: step["AddresslessPercent"]})
                result[step["Path"]].average_addressless += float(step["AddresslessPercent"][:-1])
                result[step["Path"]].variety_counts.append({file: step["Variety"]})
                result[step["Path"]].average_variety += float(step["Variety"][:-1])

    for name, entry in result.items():
        entry.average_addressless = (
            str(entry.average_addressless / len(entry.addressless_counts)) + "%"
        )
        entry.average_variety = str(entry.average_variety / len(entry.variety_counts)) + "%"

    average_variables: float = 0.0
    variable_entries = 0
    for file in os.listdir(args.input_directory + "/variables/"):
        with open(args.input_directory + "/variables/" + file, encoding="utf-8") as file_stream:
            output = yaml.safe_load(file_stream)

            average_variables += float(output["Percentage"][:-1])
            variable_entries = variable_entries + 1

    average_variables = str(average_variables / variable_entries) + "%"

    with open(args.input_directory + "/agglomerated.yml", "w") as file_stream:
        *_, last = result.items()
        yaml.dump(
            {
                "FinalStep": last[0],
                "FinalAddressless": last[1].average_addressless,
                "FinalVariety": last[1].average_variety,
                "FinalVariables": average_variables,
                "Steps": result,
            },
            file_stream,
        )

    with open(args.input_directory + "/loss_graph.csv", "w") as file_stream:
        file_stream.write("Pass,")
        first, *_ = result.items()
        for counts in first[1].addressless_counts:
            for name, _ in counts.items():
                file_stream.write(name + ",")
        file_stream.write("Average")
        file_stream.write("\n")

        for name, entry in result.items():
            file_stream.write(name + ",")
            for counts in entry.addressless_counts:
                for _, value in counts.items():
                    file_stream.write(value + ",")
            file_stream.write(entry.average_addressless)
            file_stream.write("\n")

    with open(args.input_directory + "/variety_graph.csv", "w") as file_stream:
        file_stream.write("Pass,")
        first, *_ = result.items()
        for counts in first[1].variety_counts:
            for name, _ in counts.items():
                file_stream.write(name + ",")
        file_stream.write("Average")
        file_stream.write("\n")

        for name, entry in result.items():
            file_stream.write(name + ",")
            for counts in entry.variety_counts:
                for _, value in counts.items():
                    file_stream.write(value + ",")
            file_stream.write(entry.average_variety)
            file_stream.write("\n")


if __name__ == "__main__":
    sys.exit(main())
