

# parse a yaml file

import os
import yaml


if __name__ == "__main__":
    file_path = f"{os.path.dirname(__file__)}/default.yaml"
    with open(file_path, 'r') as f:
        data = yaml.load(f, Loader=yaml.Loader)
        for server in data["servers"]:
            for key, value in server.items():
                print(f"{key}: {value}")
                print()
