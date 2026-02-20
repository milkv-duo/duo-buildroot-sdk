Technical Report: PATH Inconsistency and Image Generation on Debian 12


1. Problem Context

When using the Milk-V SDK on a clean Debian 12 installation, the image generation process fails while executing the mkdosfs utility (sh: 1: mkdosfs: not found), despite the dosfstools package being correctly installed.


2. Root Cause: PATH Privilege Separation

Debian 12 does not include /sbin and /usr/sbin directories in the default PATH for non-root users. Since the build script and the genimage tool run as a standard user, they fail to locate these system utilities.



3. Proposed Solutions

Option	  Method      	    Recommendation      	Reasoning
A	        Symbolic Link    	Preferred	            Attempts a clean fix by creating shortcuts in /usr/bin.
B	        Export PATH	      Temporary	            Adjusts the PATH only for the current terminal session.
C	        Sudo Su / Root	  Controversial	        The only method that guaranteed success in the tested environment, despite risks.


4.        Analysis of Option C (Running as Root)

Although Option A (Symbolic Link) is the logical solution, in the performed tests, the system continued to report "not found" for the standard user. Option C was the only method that allowed the build to complete.

Why might Option A have failed? (Technical Hypotheses)

    Note: The causes below are hypotheses based on observed behavior and have not been verified by full code analysis of all SDK sub-scripts:

        Environment Isolation: The genimage utility or the use of fakeroot within the SDK may be resetting the $PATH variable to a system default, ignoring symbolic links or exports made in the user's shell.

        Chroot/Container Environment: If the SDK utilizes any level of virtualization or directory isolation, it may lose access to host binaries.

Riscos of Option C:

    Permission Pollution: Generated files will be owned by Root, blocking future edits by the standard user.

    Security: Risk of executing destructive commands (rm -rf) with full privileges over the host system.


5. Permission Recovery (Post-Build)

If Option C is used, it is essential to return ownership of the generated files to your standard user to avoid "Permission Denied" errors later.

Correction command (run from the SDK root):
Bash

sudo chown -R $(whoami):$(whoami) .

(This command automatically identifies your user and group and applies them to all project files).
6. Conclusion

For users simply looking to generate the final image without diving deep into the script architecture, Option C is the fastest path, provided it is followed by the permission fix in item 5. For developers, it is recommended to investigate how genimage inherits environment variables from the host.

Technical report documented by Zeviani during development for Milk-V Duo S.
