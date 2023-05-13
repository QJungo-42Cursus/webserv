import subprocess
import time

import requests

GREEN = "\033[92m"
RED = "\033[91m"
END = "\033[0m"


def color_print(text, color):
    print(color + text + END)


def error_print(text):
    print(RED + "ERROR: \n\t- " + text + END)


def assert_get(url, expected_status_code, expected_response):
    try:
        response = requests.get(url)
        if response.status_code != expected_status_code:
            error_print("expected status code '%d', got '%d'" % (expected_status_code, response.status_code))
            return
        if response.text != expected_response:
            error_print("expected response '%s', got '%s'" % (expected_response, response.text))
            return
    except Exception as e:
        color_print("ERROR: python exception: %s" % e, RED)

    color_print("SUCCESS: GET %s" % url, "\033[92m")


def main():
    # Launch the server in a subprocess
    root_path = __file__.split("/")
    root_path = "/".join(root_path[:-3])
    test_path = __file__.split("/")
    test_path = "/".join(test_path[:-1])
    fork = subprocess.Popen([root_path + "/webserv", test_path + "/default.yaml"],
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=test_path)
    # Wait for the server to start
    time.sleep(0.2)
    # print(fork.stdout.read().decode("utf-8"))
    # Test the server
    assert_get("http://localhost:8080/", 200, "<html>index</html>")
    assert_get("http://localhost:8080/index.html", 200, "<html>index</html>")
    assert_get("http://localhost:8080/doesnotexist", 404,
               "<!DOCTYPE html><html><head><title>Error Page</title></head><body>"
               "<h1 style=\"text-align:center;\">404 Not Found</h1><p style=\"text-align:center;\">"
               "42_webserv</p></body></html>")
    fork.kill()


if __name__ == "__main__":
    main()
