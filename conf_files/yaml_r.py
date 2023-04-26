

# parse a yaml file

import os
import yaml


if __name__ == "__main__":
    file_path = f"{os.path.dirname(__file__)}/default.yaml"
    with open(file_path, 'r') as f:
        data = yaml.load(f, Loader=yaml.Loader)
        print(data)
        print()
        print(data["servers"][0]["server"])
        for server in data["servers"]:
            for key, value in server.items():
                print(f"{key}: ")
                for k, v in value.items():
                    if type(v) == list:
                        print(f"\t{k}: ")
                        for i in v:
                            print(f"\t\t{i}")
                    # print(f"\t{k}: {v}")
                print()
