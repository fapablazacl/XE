import os
import shutil
import patoolib


def extract_file(file_path, extract_to):
    try:
        patoolib.extract_archive(file_path, outdir=extract_to)
        print(f"Extracted: {file_path}")
    except Exception as e:
        print(f"Failed to extract {file_path}: {e}")

def move_folder_contents(source_folder, destination_folder):
    if not os.path.exists(source_folder) or not os.path.isdir(source_folder):
        print(f"Source folder '{source_folder}' does not exist or is not a directory.")
        return False
    if not os.path.exists(destination_folder):
        os.makedirs(destination_folder)
    for item in os.listdir(source_folder):
        src = os.path.join(source_folder, item)
        dest = os.path.join(destination_folder, item)
        if os.path.exists(dest):
            if os.path.isdir(dest):
                shutil.rmtree(dest)
            else:
                os.remove(dest)
        shutil.move(src, dest)
    os.rmdir(source_folder)
    print(f"Moved contents from '{source_folder}' to '{destination_folder}' and removed the empty source folder.")

    return True

def extract_and_process(file_path):
    extract_to = os.path.dirname(file_path)
    extract_file(file_path, extract_to)
    source_path = os.path.join(extract_to, "source")

    if move_folder_contents(source_path, extract_to):
        compressed_files = [f for f in os.listdir(extract_to) if f.endswith((".rar", ".zip"))]

        if compressed_files:
            for file in compressed_files:
                file_full_path = os.path.join(extract_to, file)
                if file_full_path == file_path:
                    continue

                extract_file(file_full_path, extract_to)
        else:
            print("No compressed further files where found.")

    print("Processing complete.")

if __name__=='__main__':
    extract_and_process('/Users/fapablaza/Dropbox/GameDev/Capybaria/raw-assets/amongus-02-test/among-us (2).zip')
