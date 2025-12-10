import subprocess
import time
import os
import signal

env = os.environ.copy()
env["LD_LIBRARY_PATH"] = "/home/container/library"

restarts = 0
MAX_RESTARTS = 5

subprocess.run(["chmod", "+x", "/home/container/bot"])
while restarts < MAX_RESTARTS:
    print("Запускаем программу...")
    process = subprocess.Popen(["/home/container/bot"], env=env)
    
    rc = process.wait()
    
    if rc < 0:
        sig = -rc
        print(f"Упала по сигналу: {signal.Signals(sig).name}")
    else:
        print(f"Завершилась с кодом {rc}")
    

    restarts += 1
    print("Перезапуск через 5 секунд...\n")
    time.sleep(5)

print("⛔ Лимит перезапусков исчерпан")