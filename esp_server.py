from http.server import HTTPServer, BaseHTTPRequestHandler
import sys
import re
import datetime

tcre = re.compile(r'tc=([0-9]+\.[0-9]+)')
tfre = re.compile(r'tf=([0-9]+\.[0-9]+)')
hpre = re.compile(r'hp=([0-9]+\.[0-9]+)')

class ESPHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        print(self.path)
        tc = float(tcre.search(self.path).group(1))
        tf = float(tfre.search(self.path).group(1))
        hp = float(hpre.search(self.path).group(1))
        print(tc,tf,hp)
        w = open('esp.log','a+')
        w.write(f"{datetime.datetime.now()} {tc} {tf} {hp}\n")
        w.close()
        self.send_response(200)
        self.end_headers()
        self.flush_headers()

def run():
    addy = ('0.0.0.0', 8030)
    server = HTTPServer(addy, ESPHandler)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        sys.exit(0)

if __name__ == '__main__':
    run()


