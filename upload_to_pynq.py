import os
import sys
from paramiko import SSHClient, AutoAddPolicy
from scp import SCPClient

IP = '192.168.2.99'
USER = 'xilinx'
PASS = 'xilinx'
SRC_DIR = 'pynq-embedded'
DEST_DIR = '/home/xilinx/jupyter_notebooks/fpga-edge-ai-plant-disease'

def progress(filename, size, sent):
    sys.stdout.write(f"\rUploading {filename.decode('utf-8') if isinstance(filename, bytes) else filename}: {float(sent)/float(size)*100:.2f}%   ")
    sys.stdout.flush()

def main():
    print(f"Connecting to PYNQ board at {IP}...")
    ssh = SSHClient()
    ssh.set_missing_host_key_policy(AutoAddPolicy())
    
    try:
        ssh.connect(IP, username=USER, password=PASS, timeout=10)
    except Exception as e:
        print(f"\n[Error] Failed to connect to {IP}: {e}")
        print("Please make sure the board is turned on and connected to the network.")
        return
        
    print("Connected successfully! Creating remote directory...")
    # Create the base directory on the board
    ssh.exec_command(f'mkdir -p {DEST_DIR}')
    
    print(f"Uploading '{SRC_DIR}' folder...")
    with SCPClient(ssh.get_transport(), progress=progress) as scp:
        try:
            # We upload the contents inside pynq-embedded to the destination
            scp.put(SRC_DIR, recursive=True, remote_path='/home/xilinx/jupyter_notebooks/fpga-edge-ai-plant-disease')
            print("\n\n[Success] Upload completed successfully!")
            print("You can now open your browser to http://192.168.2.99 and check the 'fpga-edge-ai-plant-disease' folder.")
        except Exception as e:
            print(f"\n\n[Error] Upload failed: {e}")
            
    ssh.close()

if __name__ == '__main__':
    main()
