import os
import shutil
import patoolib
from pathlib import Path

archive_exts = (".rar", ".zip")

def extract_file(file_path, extract_to):
    try:
        patoolib.extract_archive(file_path, outdir=extract_to)
        print(f'Extracted: "{os.path.basename(file_path)}"')
    except Exception as e:
        print(f"Failed to extract {file_path}: {e}")

def move_folder_contents(source_folder, destination_folder):
    if not os.path.exists(source_folder) or not os.path.isdir(source_folder):
        raise FileNotFoundError(f'The {source_folder} folder does not exists, or it is not a folder.')

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

def extract_and_process(archive_path, extraction_path, cleanup_temporaries=True):
    if not os.path.exists(archive_path) or not os.path.isfile(archive_path):
        raise FileNotFoundError(f'The archive at "{archive_path}" does not exists, or it is not a file.')

    extract_file(archive_path, extraction_path)

    # check if the archive contains a source subfolder
    extracted_files = []
    source_path = os.path.join(extraction_path, "source")

    if os.path.exists(source_path):
        move_folder_contents(source_path, extraction_path)
        # checks if there's any additionally compressed assets in the extracted archive
        compressed_files = [f for f in os.listdir(extraction_path) if f.endswith(archive_exts)]

        if compressed_files:
            for file in compressed_files:
                file_full_path = os.path.join(extraction_path, file)
                if file_full_path == archive_path:
                    continue

                extract_file(file_full_path, os.path.dirname(file_full_path))

                extracted_files.append(file_full_path)
        else:
            print("No compressed files were found in the original archive file")

    textures_path = os.path.join(extraction_path, "textures")
    if os.path.exists(textures_path):
        move_folder_contents(textures_path, extraction_path)

    if cleanup_temporaries:
        if os.path.exists(textures_path):
            print(f'Removing folder "{textures_path}"')
            shutil.rmtree(textures_path)

        if os.path.exists(source_path):
            print(f'Removing folder "{source_path}"')
            shutil.rmtree(source_path)

        for file in extracted_files:
            print(f'Removing file "{file}"')
            os.remove(file)

    print("Processing complete.")

if __name__=='__main__':
    test_archive_path = '/Users/fapablaza/Dropbox/GameDev/Capybaria/raw-assets/amongus-03/among-us-character-model.zip'
    extraction_path = os.path.join(os.path.dirname(test_archive_path), Path(test_archive_path).stem)

    try:
        extract_and_process(test_archive_path, extraction_path)
    except Exception as e:
        print(f'Error while extracting "{test_archive_path}":\n{e}')
        exit(1)
