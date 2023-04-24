from time import sleep
import requests

if __name__ == "__main__":
    print("python client started")
    while True:
        try:
            print("python req")
            response = requests.get("http://127.0.0.1:8080")
            print("python rep:", response.text)
        except requests.exceptions.ConnectionError as e:
            print(f"Connection error: \"{e}\"")
        
        sleep(2)

""" Python will send the following request to the server:

GET / HTTP/1.1
Host: 127.0.0.1:8080
User-Agent: python-requests/2.28.2
Accept-Encoding: gzip, deflate
Accept: */*
Connection: keep-alive

"""
