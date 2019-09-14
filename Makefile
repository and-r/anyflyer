.PHONY: clean All

All:
	@echo "----------Building project:[ anyflyer-project - Release ]----------"
	@cd "anyflyer-project" && "$(MAKE)" -f  "anyflyer-project.mk" && "$(MAKE)" -f  "anyflyer-project.mk" PostBuild
clean:
	@echo "----------Cleaning project:[ anyflyer-project - Release ]----------"
	@cd "anyflyer-project" && "$(MAKE)" -f  "anyflyer-project.mk" clean
