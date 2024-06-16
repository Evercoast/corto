from encodings import utf_8
import subprocess
import argparse
import os
import sys
import shutil
import re

def codesign(lib_path):
    print(f'codesign: {os.path.basename(lib_path)}')
    result = subprocess.check_output(['security', 'find-identity', '-v', '-p', 'codesigning'])
    match = re.search(r'\d+\)\s+([a-zA-Z0-9]+)[^@]+@evercoast.com', result.decode('utf8'))
    if match:
        cert = match[1]

        subprocess.check_call([
            'codesign',
            '--force',
            '--timestamp',
            '--sign', cert,
            lib_path
        ])

        print(subprocess.check_output([
            'codesign',
            '-vvvv',
            lib_path
        ]).decode('utf8'))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument("filename")
    args = parser.parse_args()
    
    codesign(args.filename)