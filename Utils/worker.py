#!/usr/bin/env python3

# Этот скрипт должен находится рядом с исполняемым файлом 

import sys, json
from yt_dlp import YoutubeDL
import re

def clean_url(url):
    if 'playlist' not in url and 'list=' in url:
        return re.sub(r"(\?list=[^&]+)", "", url)
    return url

url = sys.argv[1]

ydl_opts = {
    "quiet": True,
    "skip_download": True,
    "format": "bestaudio/best",
    "no_warnings": True,
}

with YoutubeDL(ydl_opts) as ydl:
    info = ydl.extract_info(clean_url(url), download=False)
    result = {
        "title": info.get("title"),
        "author": info.get("uploader"),
        "duration": info.get("duration"),
        "stream_url": info["url"],
    }

print(json.dumps(result), file=sys.stdout)