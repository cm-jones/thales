# Contributing to Thales

We welcome contributions of all kinds, including bug reports, feature requests, and code contributions. For bug reports and feature requests, [open an issue](https://github.com/cm-jones/thales/issues/new). For code contributions, follow these steps:

1. Fork the repository.

   - Navigate to the [repository](https://github.com/cm-jones/thales) on GitHub.
   - Click the "Fork" button in the top-right corner of the page.

2. Clone your fork.

```sh
git clone https://github.com/YOUR_USERNAME/thales.git
cd thales
```

3. Create a new branch with a descriptive name.

```sh
git checkout -b descriptive-branch-name
```

4. Make your changes following the project's coding standards.

5. Commit and push your changes.

```sh
git add .
git commit -m "Descriptive commit message"
git push origin descriptive-branch-name
```

6. Create a pull request.

   - Navigate to your forked repository on GitHub.
   - Click the "Compare & pull request" button next to your branch.
   - If your pull request addresses an open issue, reference the issue number.
   - Provide a detailed description of your changes and submit the pull request.

### Coding Style

To maintain code consistency, we use clang-format with the following settings:

- BasedOnStyle: Google
- IndentWidth: 4
- ColumnLimit: 80

Please ensure that your code adheres to this style. You can format your code using clang-format before committing by running the following script:

```sh
chmod +x /scripts/format.sh # If the script isn't already executable
./scripts/format.sh
```

Alternatively, you can set up your editor or IDE to format the code automatically on save. Refer to your editor or IDE documentation for instructions on how to integrate clang-format.

## Getting Help

If you need any help, feel free to [open an issue](https://github.com/cm-jones/thales/issues/new). We appreciate your feedback and contributions!

## Additional Resources

- [Contributing to a Project on GitHub](https://docs.github.com/en/get-started/exploring-projects-on-github/contributing-to-a-project)
- [Forking Projects](https://guides.github.com/activities/forking/)
- [Creating a Pull Request](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/creating-a-pull-request)
