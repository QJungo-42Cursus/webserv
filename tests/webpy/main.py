# un server python qui ecoute un port et qui renvoie la requete brut dan un autre serveur

# Python 3 server example
from http.server import BaseHTTPRequestHandler, HTTPServer
import time
import requests
from requests_toolbelt.utils import dump

hostName = "localhost"
serverPort = 8000


class MyServer(BaseHTTPRequestHandler):
    def do_GET(self):
        print("tester send:")
        print("=======================================")
        print(self.headers)
        print("=======================================")
        answer = requests.get("http://localhost:8080" + self.path, headers=self.headers)
        print("nginx response:")
        print("=======================================")
        data = dump.dump_response(answer)
        print(data.decode('utf-8'))
        print("=======================================")
        self.send_response(answer.status_code)
        for header in answer.headers:
            self.send_header(header, answer.headers[header])
        self.end_headers()
        for chunk in answer.iter_content(chunk_size=1024):
            self.wfile.write(chunk)

    def do_POST(self):
        print("tester send:")
        print("=======================================")
        print(self.requestline)
        print("=======================================")
        content_length = int(self.headers['Content-Length'])
        body = self.rfile.read(content_length)
        answer = requests.post("http://localhost:8080" + self.path, headers=self.headers, data=body)
        print("nginx response:")
        print("=======================================")
        data = dump.dump_all(answer)
        print(data.decode('utf-8'))
        print("=======================================")
        self.send_response(answer.status_code)
        for header in answer.headers:
            self.send_header(header, answer.headers[header])
        self.end_headers()
        for chunk in answer.iter_content(chunk_size=1024):
            self.wfile.write(chunk)

    def do_DELETE(self):
        print("tester send:")
        print("=======================================")
        print(self.requestline)
        print("=======================================")
        answer = requests.delete("http://localhost:8080" + self.path, headers=self.headers)
        print("nginx response:")
        print("=======================================")
        data = dump.dump_all(answer)
        print(data.decode('utf-8'))
        print("=======================================")
        self.send_response(answer.status_code)
        for header in answer.headers:
            self.send_header(header, answer.headers[header])
        self.end_headers()
        for chunk in answer.iter_content(chunk_size=1024):
            self.wfile.write(chunk)

    def do_PUT(self):
        print("tester send:")
        print("=======================================")
        print(self.requestline)
        print("=======================================")
        content_length = int(self.headers['Content-Length'])
        body = self.rfile.read(content_length)
        answer = requests.put("http://localhost:8080" + self.path, headers=self.headers, data=body)
        print("nginx response:")
        print("=======================================")
        data = dump.dump_all(answer)
        print(data.decode('utf-8'))
        print("=======================================")
        self.send_response(answer.status_code)
        for header in answer.headers:
            self.send_header(header, answer.headers[header])
        self.end_headers()
        for chunk in answer.iter_content(chunk_size=1024):
            self.wfile.write(chunk)

    def do_HEAD(self):
        print("tester send:")
        print("=======================================")
        print(self.requestline)
        print("=======================================")
        answer = requests.head("http://localhost:8080" + self.path, headers=self.headers)
        print("nginx response:")
        print("=======================================")
        data = dump.dump_all(answer)
        print(data.decode('utf-8'))
        print("=======================================")
        self.send_response(answer.status_code)
        for header in answer.headers:
            self.send_header(header, answer.headers[header])
        self.end_headers()
        for chunk in answer.iter_content(chunk_size=1024):
            self.wfile.write(chunk)


if __name__ == "__main__":
    webServer = HTTPServer((hostName, serverPort), MyServer)
    print("Server started http://%s:%s" % (hostName, serverPort))
    try:
        webServer.serve_forever()
    except KeyboardInterrupt:
        pass
    webServer.server_close()
    print("Server stopped.")
