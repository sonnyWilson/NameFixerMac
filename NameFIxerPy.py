import os


def find_matching_files(directory, target_char):
    """Finds all files in directory (and subdirectories) containing target_char."""
    matching_files = []
    for root, _, files in os.walk(directory):
        for filename in files:
            if target_char in filename:
                matching_files.append((root, filename))
    return matching_files


def rename_files(matching_files, target_char, replacement_char, dry_run=False):
    """Renames files by replacing target_char with replacement_char."""
    renamed_count = 0
    for root, old_filename in matching_files:
        new_filename = old_filename.replace(target_char, replacement_char)
        old_path = os.path.join(root, old_filename)
        new_path = os.path.join(root, new_filename)

        if dry_run:
            print(f"[DRY RUN] Would rename: {old_filename} -> {new_filename}")
        else:
            try:
                os.rename(old_path, new_path)
                print(f"Renamed: {old_filename} -> {new_filename}")
                renamed_count += 1
            except OSError as e:
                print(f"Error renaming {old_filename}: {e}")

    return renamed_count


def main():
    target_dir = input("Enter directory path (press Enter for current folder): ").strip()
    if not target_dir:
        target_dir = "."

    if not os.path.isdir(target_dir):
        print(f"Error: Directory '{target_dir}' does not exist.")
        return

    target_char = input("Enter target character to search for: ")
    if not target_char:
        print("Error: Target character cannot be empty.")
        return

    replacement_char = input("Enter replacement character: ")

    # Find matching files
    matches = find_matching_files(target_dir, target_char)

    if not matches:
        print(f"\nNo files found containing '{target_char}'.")
        return

    print(f"\nFound {len(matches)} matching file(s):")
    for root, filename in matches:
        print(f" - {os.path.join(root, filename)}")

    # Dry run preview
    print("\n--- Preview of Changes ---")
    rename_files(matches, target_char, replacement_char, dry_run=True)

    # Confirmation
    confirm = input("\nProceed with renaming? (y/n): ").strip().lower()
    if confirm == "y":
        print("\nRenaming files...")
        count = rename_files(matches, target_char, replacement_char, dry_run=False)
        print(f"\nFinished! Successfully renamed {count} file(s).")
        print(f"Finished! Successfully renamed {count} file(s).")
    else:
        print("\nOperation cancelled. No files were changed.")


if __name__ == "__main__":
    main()