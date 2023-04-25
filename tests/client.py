from time import sleep
import requests

if __name__ == "__main__":
    i = 0
    while True:
        try:
            print(f"python req {i}")
            response = requests.get("http://127.0.0.1:8080")
            print("python rep:", response.text)
        except requests.exceptions.ConnectionError as e:
            print(f"Connection error: \"{e}\"")
        i += 1
        sleep(0.1)

""" Python will send the following request to the server:

GET / HTTP/1.1
Host: 127.0.0.1:8080
User-Agent: python-requests/2.28.2
Accept-Encoding: gzip, deflate
Accept: */*
Connection: keep-alive

"""
