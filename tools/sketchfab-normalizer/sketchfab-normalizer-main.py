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

def extract_and_process(archive_path, cleanup_temporaries=True):
    archive_exts = [".rar", ".zip"]

    extraction_path = os.path.dirname(archive_path)
    extract_file(archive_path, extraction_path)
    source_path = os.path.join(extraction_path, "source")

    extracted_files = []

    if move_folder_contents(source_path, extraction_path):
        # checks if there's any additionally compressed assets in the extracted archive
        compressed_files = [f for f in os.listdir(extraction_path) if f.endswith(archive_exts)]

        if compressed_files:
            for file in compressed_files:
                file_full_path = os.path.join(extraction_path, file)
                if file_full_path == archive_path:
                    continue

                extract_file(file_full_path, extraction_path)

                extracted_files.append(file_full_path)
        else:
            print("No compressed further files where found.")

    textures_path = os.path.join(extraction_path, "textures")
    if os.path.exists(textures_path):
        move_folder_contents(textures_path, extraction_path)

    if cleanup_temporaries:
        if os.path.exists(textures_path):
            shutil.rmtree(textures_path)

        if os.path.exists(source_path):
            shutil.rmtree(source_path)

        for file in extracted_files:
            os.remove(file)

    print("Processing complete.")

if __name__=='__main__':
    extract_and_process('/Users/fapablaza/Dropbox/GameDev/Capybaria/raw-assets/amongus-03-test/among-us-character-model.zip')
