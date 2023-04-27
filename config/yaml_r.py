

# parse a yaml file

import os
import yaml


if __name__ == "__main__":
    file_path = f"{os.path.dirname(__file__)}/default.yaml"
    with open(file_path, 'r') as f:
        data = yaml.load(f, Loader=yaml.Loader)
        print(data["servers"])
        print()
        print(data["servers"][0]["server"])
