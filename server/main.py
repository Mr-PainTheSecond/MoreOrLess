import sqlite3
import query_types as qt
from messages import Messages
import random
import threading
import requests
import html
import yt_dlp as ytd
import googleapiclient.discovery as google
import os

class YouTubeData():
    def __init__(self, firstIndex):
        self.threads: list[threading.Thread] = list()
        self.urls = []
        self.firstIndex = firstIndex
        self.lock = threading.Lock()
        self.path = "../assets/images/temp/"
        self.filesNames = []
        self.viewCounts = []
        self.youtube = google.build("youtube", "v3", developerKey=os.getenv("YOUTUBE_API_KEY"))

    def downloadThumbnail(self, url, count):
        thumnailUrl = self.getThumnbnailUrl(url, count)
        if not thumnailUrl:
            return
        rResponse = requests.get(thumnailUrl)
        with open(f"{self.path}test{count + self.firstIndex}.png", "wb") as file:
            file.write(rResponse.content)
            with self.lock:
                self.filesNames.append(f"{self.path}test{count + self.firstIndex}.png")
        
        
    def getThumnbnailUrl(self, url, count):
        options = {
            "quiet": True,
            "no_warnings": True
        }
        with ytd.YoutubeDL(options) as ydl:
            try:
                thumbnailUrl = ydl.extract_info(url, download=False)
                return thumbnailUrl.get("thumbnail")
            except Exception:
                self.viewCounts.pop(count)
                return None
        

    def getFromYoutube(self, result):
        ytId = random.choice(result)[4]
        request = self.youtube.videos().list(
            part= "statistics",
            id = ytId
        )
        
        response = request.execute()
        if response and response["items"]:
            try:
                self.viewCounts.append(response["items"][0]["statistics"]["viewCount"])
            except KeyError:
                return 
            self.urls.append(f"https://www.youtube.com/watch?v={ytId}")

    def handleCategory(self):
        category = random.choice(qt.typeList)
        result = None
        if category == "gaming":
            self.cursor.execute("SELECT * FROM youtube WHERE category_id = 20")
        elif category == "beauty":
            self.cursor.execute("SELECT * FROM youtube WHERE category_id = 26")
        elif category == "music":
            self.cursor.execute("SELECT * FROM youtube WHERE category_id = 10")
        elif category == "random":
            self.cursor.execute("SELECT * FROM youtube")
        
        result = self.cursor.fetchall()
        self.getFromYoutube(result)
    
    def getData(self):
        self.connection = sqlite3.connect("youtube.db")
        self.cursor = self.connection.cursor()
        for i in range(20):
            self.handleCategory()
    
    def getThumbnails(self):
        for j in range(len(self.urls)):
            newThread = threading.Thread(None, self.downloadThumbnail, args=(self.urls[j], j))
            self.threads.append(newThread)
        
        for thread in self.threads:
            thread.start()
        
        for thread in self.threads:
            thread.join()
        

        for url in self.urls:
            print(url)

        self.clearStraglers()
    
    def clearStraglers(self):
      tempFiles = list(self.filesNames)
      for a in range(len(self.filesNames)):
            for elements in tempFiles:
                if not elements:
                    self.filesNames.pop(a)
                    continue
                if (elements == f"{self.path}test{a + self.firstIndex}.png"):
                    self.filesNames[a] = elements
                    break
    
    
    def cleanData(self):
        for image in self.filesNames:
            os.remove(image)
        
if __name__ == "__main__":
    firstIndex = 0
    messageManager = Messages()
    while True:
        dataManager = YouTubeData(firstIndex)
        dataManager.getData()
        dataManager.getThumbnails()
        messageManager.findConnection()    
        messageManager.sendYTData(dataManager.viewCounts, dataManager.filesNames)
        firstIndex = (len(dataManager.filesNames) + firstIndex) % 100
       
        
        
