# Contributing to Big JSON Tree Viewer

Feel free to submit bug reports or suggestions/feature requests to the issue tracker, but please
first check existing issues and [TODO.txt](./TODO.txt).

Given the very early state of this tool, multiple contributors are likely to bump heads. If
you'd like to contribute, please first comment on an issue and state that you wish to help.

The code is organized like this:
* To get started, open the .sln in the src directory with Visual Studio 2022
* Root directory contains generated MFC template files with minimal modifications.
** Many of these files are not used currently but have not been cleaned up yet.
* Bread and Butter directories contain most of the actual logic. Butter is UI related.
* The external dir contains third party libraries
