
# ============================================================================
# Generic build targets and recipes for SuperNOVAS.
# 
# You can include this in your Makefile also.
# ============================================================================


# Regular object files
$(OBJ)/%.o: $(SRC)/%.c $(OBJ) Makefile
	$(CC) -o $@ -c $(CPPFLAGS) $(CFLAGS) $<

# Shared library recipe
$(LIB)/%.$(SOEXT).$(SO_VERSION): | $(LIB)
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $^ $(SHARED_FLAGS) $(SONAME_FLAG)$(notdir $@) $(LDFLAGS)

# Unversioned shared libs (for linking against)
$(LIB)/lib%.$(SOEXT):
	@rm -f $@
	( cd $(dir $@); ln -s $(notdir $<) $(notdir $@) )

# Static library recipe
$(LIB)/%.a:
	@$(MAKE) $(LIB)
	ar -rc $@ $^
	ranlib $@

# Create sub-directories for build targets
$(OBJ) $(LIB) $(BIN) apidoc:
	mkdir -p $@

# Remove intermediate files locally
.PHONY: clean-local
clean-local:
	@rm -rf obj

# Remove all locally built files, effectively restoring the repo to its 
# pristine state
.PHONY: distclean-local
distclean-local: clean-local
	@rm -rf bin lib infer-out

# Remove intermediate files (general)
.PHONY: clean
clean: clean-local

# Remove intermediate files (general)
.PHONY: distclean
distclean: distclean-local

CHECK_DIR ?= $(SRC)

# Static code analysis using 'cppcheck'
.PHONY: analyze
analyze:
	@echo "   [analyze]"
	@cppcheck $(CPPFLAGS) $(CHECKOPTS) $(CHECK_DIR)

# Static code analysis viacat Facebook's infer
.PHONY: infer
infer: clean
	infer run -- $(MAKE) shared




