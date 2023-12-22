PostBuildSteps: $(TARGET)
	rebase -B 0x10000 -X . $(TARGET)
	copy $(TARGET) $(WINDIR)\system32\drivers
