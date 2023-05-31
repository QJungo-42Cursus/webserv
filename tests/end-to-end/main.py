import subprocess

import requests

GREEN = "\033[92m"
RED = "\033[91m"
BLUE = "\033[94m"
END = "\033[0m"

ERROR_404 = "<!DOCTYPE html><html><head><title>Error Page</title></head><body>" \
            "<h1 style=\"text-align:center;\">404 Not Found</h1><p style=\"text-align:center;\">" \
            "42_webserv</p></body></html>"

HOST = "http://localhost:8080"


def color_print(text, color):
    print(color + text + END)


def error_print(text, url=None):
    print(RED + "ERROR: \n\t- " + text, end=" ")
    if url:
        print("for URL: " + url, end="")
    print(END)


def assert_get(url, expected_status_code, expected_response):
    try:
        response = requests.get(url)
        if response.status_code != expected_status_code:
            error_print("expected status code '%d', got '%d'" % (expected_status_code,
                                                                 response.status_code), url)
            return
        if response.text != expected_response:
            error_print("expected response '%s', got '%s'" % (expected_response, response.text))
            return
    except Exception as e:
        color_print("ERROR: python exception: %s" % e, RED)

    color_print("SUCCESS: GET %s" % url, GREEN)


def assert_post(url, expected_status_code, expected_response):
    try:
        response = requests.post(url)
        if response.status_code != expected_status_code:
            error_print("expected status code '%d', got '%d'" % (expected_status_code,
                                                                 response.status_code), url)
            return
        if response.text != expected_response:
            error_print("expected response '%s', got '%s'" % (expected_response, response.text))
            return
    except Exception as e:
        color_print("ERROR: python exception: %s" % e, RED)

    color_print("SUCCESS: POST %s" % url, GREEN)


def assert_put(url, expected_status_code, expected_response):
    try:
        response = requests.put(url)
        if response.status_code != expected_status_code:
            error_print("expected status code '%d', got '%d'" % (expected_status_code,
                                                                 response.status_code), url)
            return
        if response.text != expected_response:
            error_print("expected response '%s', got '%s'" % (expected_response, response.text))
            return
    except Exception as e:
        color_print("ERROR: python exception: %s" % e, RED)

    color_print("SUCCESS: PUT %s" % url, GREEN)


def assert_delete(url, expected_status_code, expected_response):
    try:
        response = requests.delete(url)
        if response.status_code != expected_status_code:
            error_print("expected status code '%d', got '%d'" % (expected_status_code,
                                                                 response.status_code), url)
            return
        if response.text != expected_response:
            error_print("expected response '%s', got '%s'" % (expected_response, response.text))
            return
    except Exception as e:
        color_print("ERROR: python exception: %s" % e, RED)

    color_print("SUCCESS: DELETE %s" % url, GREEN)


def assert_redirect(url, expected_status_code, expected_response, expected_history_length=1,
                    catch_infinite_redirect=False):
    try:
        response = requests.get(url)
        if len(response.history) != expected_history_length:
            error_print(
                f"expected {expected_history_length} redirect, got {len(response.history)}"
                f" (and code {response.status_code})", url)
            return
        response = response.history[0]
        if response.status_code != expected_status_code:
            error_print(f"expected status code '{expected_status_code}', got '{response.status_code}'", url)
            return
        if response.text != expected_response:
            error_print("expected response '%s', got '%s'" % (expected_response, response.text))
    except requests.exceptions.TooManyRedirects:
        if catch_infinite_redirect:
            color_print(f"SUCCESS: caught infinite redirect {url}", GREEN)
            return
        else:
            error_print("caught infinite redirect", url)
    except Exception as e:
        error_print("python exception: %s" % e, url)
    else:
        color_print("SUCCESS: GET redirect %s" % url, GREEN)


def main():
    # Get the root path of the project
    root_path = __file__.split("/")
    root_path = "/".join(root_path[:-3])
    test_path = __file__.split("/")
    test_path = "/".join(test_path[:-1])

    # Launch the server in a subprocess
    fork = subprocess.Popen([root_path + "/webserv", test_path + "/default.yaml"],
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=test_path)

    # Wait for the server to start
    try:
        # server_log = fork.stderr.read().decode("utf-8")
        exit_code = fork.wait(0.2)
        error_print("server exited with code %d" % exit_code)
        # color_print("== server log ==", BLUE)
        # color_print(server_log, BLUE)
        # color_print("====", BLUE)
        return
    except subprocess.TimeoutExpired:
        color_print("server started", BLUE)

    # Test the server
    assert_get(HOST, 200, "<html>index</html>")
    assert_get(HOST + '/', 200, "<html>index</html>")
    assert_get(HOST + '   /', 200, "<html>index</html>")
    assert_get(HOST + '/   ', 200, "<html>index</html>")
    assert_get(HOST + "/index.html", 200, "<html>index</html>")
    assert_get(HOST + "/index", 404, ERROR_404)
    assert_get(HOST + '/redirect_to_index', 200, "<html>index</html>")
    assert_get(HOST + "/doesnotexist", 404, ERROR_404)
    assert_get(HOST + "/photo/photo1", 200, "photo1")
    assert_redirect(HOST + "/google", 302, "", 1)
    assert_redirect(HOST + '/redirect_to_google', 302, '', 2)
    assert_redirect(HOST + "/recursive_redirect", 302, "", 3, True)
    assert_redirect(HOST + "/mutual_redirect", 302, "", 2, True)
    print('cgi call: ')
    assert_get(HOST + "/api/call.ppp", 200, "HI !")
    assert_get(HOST + "/api/404.php", 200, "HI !")
    fork.kill()


if __name__ == "__main__":
    main()
