
# Pulmonary Docs
 Easy API and Code Documentation Toolchain For C++ Projects. This documentation solution uses Doxygen +Shpinx + Breathe + Exhale to create user generated docs and auto code docs.
 You need to write the doxygen documentation in your code, and you can create your own documentation for Tutorials, Instructions etc. This toolchain will produce a well structures cross-linked website (html) for your documentation

 ## Installation Instructions
 The tools used for this project are all cross platform, so you can use any operating system you like. You should be able to use this with any IDE tha supports CMake Projects, but it's only tested on CLion.
## 1. Install Dependencies
### Install Doxygen
Linux : `apt-get install -y doxygen`
Windows : `choco install doxygen.install`
MacOs : `brew install doxygen`

[Doxygen Documentation](https://www.doxygen.nl/index.html)
### Install Sphinx
Linux : `$ apt-get install python3-sphinx`
Mac Os : `$ brew install sphinx-doc`
Windows : `$ pip install -U sphinx`
> There is no way to install sphinx in windows through a package manager on an installation file. Install Python 3.6+ first and then install sphinx using PIP.

[Sphinx Documentation](https://www.sphinx-doc.org/en/master/usage/installation.html)

### Install Python 3.6+ and PIP
I recommend installing with pyenv for better version management.
[PyEnv Documentation](https://github.com/pyenv/pyenv)

### Install Breathe
`$ pip install breathe`

[Breathe Documentation](https://breathe.readthedocs.io/en/latest/)

### Install Exhale
`$pip install exhale`

[Exhale Documentation](https://exhale.readthedocs.io/en/latest/)

## 2. Install Configuration Files

 - Copy and paste the doc folder from this repository to the **root directory** of your C++ project.
 - Add the following lines in your CMakeList.txt

	```cmake
	set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/docs/cmake" 			${CMAKE_MODULE_PATH})
	add_subdirectory(docs)'
	```
 - Right click at your CmakeList.txt and select **Reload CMake Project**


You should now be able to see the new **Generate_Docs** and Doxygen configurations in your build confugurations list.
If you dont see these configurations select *Edit Configurations* click the *Add new configuration*  and select Doxygen or Generate_Docs for the target.

Build the generate docs Configuration

Your new HTML Documentation Pages are located inside  *./docs/output/sphinx*

## Setting up the IDE
Doxygen will automatically document your code based on your doxygen comments
You can create API docs/Tutorials/ Instructions and Examples by creating new rst files inside docs/src
Changing the Name of the site

## How to Document your project
 To learn how to **write doxygen** comments in your code please read [Here](https://www.doxygen.nl/manual/docblocks.html)
 For intruction on how to write **restructured Text** read [Here](https://docutils.readthedocs.io/en/sphinx-docs/user/rst/cheatsheet.html)
If you  to **cross-link your code** inside your user documentation read [Here](#TODO)
## Future Plans

 - [ ] 1.We could create a package and a CLI to make the installation and setup even easier and more modular .With  installation arguments we could add:
	  -Programming language selection C/C++
	 - Project name
	 - Documentation  type selection rst/md
	 - Theme selection

 - [ ] 2. Set build arguments for creating html/xml/latex output
 - [ ] 3. Create a version controled output so we can see documentation for every major code version
