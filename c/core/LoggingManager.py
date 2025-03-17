import time
import sys

class LoggingManager():
    def __init__(self, name):
        self.name = name

    #def construct_msg(self, severity, text):
    #    timestamp = time.strftime("%H:%M:%S", time.localtime())
    #    return f"{timestamp} [{severity.upper()}] {self.name.upper()}: {text}"

    def log(self, *args, **kwargs):
        print(*args, **kwargs, file=sys.stderr)

    def info(self, message):
        self.log(message)

    def warn(self, message):
        self.log(message)
    
    def error(self, message):
        self.log(message)
    
    def ok(self, message):
        self.log(message)
    