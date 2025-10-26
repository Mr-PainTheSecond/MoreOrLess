import zmq
import time
import os
from rich import print

class Messages:
    def __init__(self):
        self.context = zmq.Context()
        self.path = "../assets/images/temp/"
        self.socket = self.context.socket(zmq.REP)
        self.socket.bind("tcp://*:5555")
    
    def findConnection(self):
        print("Trying to find connection...")
        response = str(self.socket.recv())
        print("[green]Established a connection")
        return True
    
    def sendYTData(self, views, fileNames):
        for view, file in zip(views, fileNames, strict=True):
            if not view or not file:
                continue
            self.socket.send_string(view)
            self.socket.recv()
            self.socket.send_string(file)
            self.socket.recv()
        
        self.socket.send_string("-1")
    
    def talkToClient(self, *args):
        files = list(args)
        while files:
            response = self.socket.recv()
            if "Delete" in str(response):
                os.remove(files[0])
                files.pop(0)
                self.socket.send_string("roger")
            elif "Terminate" in str(response):
                self.socket.close()
                self.socket = self.context.socket(zmq.REP)
                self.socket.bind("tcp://*:5555")
                return