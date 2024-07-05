import requests

def send_request():
    url = 'http://localhost:8080'
    response = requests.get(url)
    print('Response from server:', response.text)

if __name__ == "__main__":
    send_request()
