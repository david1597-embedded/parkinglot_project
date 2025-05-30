import multiprocessing
import subprocess
import time

def run_script(script_name):
    """Executing designated python Scripts!"""
    try:
        print(f"{script_name} start")
        subprocess.run(["python", script_name], check=True)
        print(f"{script_name} completed")
    except subprocess.CalledProcessError as e:
        print(f"{script_name} failed to start: {e}")
    except KeyboardInterrupt:
        print(f"{script_name} termainated by user")

if __name__ == '__main__':
   
    scripts = ["cctv.py", "data.py","login.py"]

    
    processes = []


    for script in scripts:
        process = multiprocessing.Process(target=run_script, args=(script,))
        processes.append(process)
        process.start()

    try:
      
        for process in processes:
            process.join()
    except KeyboardInterrupt:
        print("Received to exit program. All process Terminated...")
        for process in processes:
            process.terminate()
            process.join()

    print("Completed Execution!!")
