from time import sleep
import requests

if __name__ == "__main__":
	while True:
		try:
			response = requests.get("http://127.0.0.1:8080")
		except requests.exceptions.ConnectionError as e:
			print(f"Connection error: \"{e}\"")
			# exit()
		sleep(10)
		print("python rep:", response.text)

""" Python will send the following request to the server:

GET / HTTP/1.1
Host: 127.0.0.1:8080
User-Agent: python-requests/2.28.2
Accept-Encoding: gzip, deflate
Accept: */*
Connection: keep-alive

"""