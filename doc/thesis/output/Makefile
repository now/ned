# contents: Master's Thesis Main Makefile.
#
# Copyright © 2005 Nikolai Weibull <work@rawuncut.elitemail.org>

INC_FILES = \
	    ../include/colors.mp \
	    ../include/commands.tex \
	    ../include/includes.tex \
	    ../include/layout.tex \
	    ../include/regex-syntax-trees.mp

TEX_FILES = \
	    ../masters-project.tex \
	    ../thesis/thesis.tex \
	    ../thesis/appendices/definitions.tex \
	    ../thesis/appendices/posix-regex.tex \
	    ../thesis/appendices/regex-editors.tex \
	    ../thesis/chapters/abbreviations.tex \
	    ../thesis/chapters/bibliography.tex \
	    ../thesis/chapters/conclusion.tex \
	    ../thesis/chapters/construction.tex \
	    ../thesis/chapters/dedication.tex \
	    ../thesis/chapters/definitions.tex \
	    ../thesis/chapters/implementation-regexes.tex \
	    ../thesis/chapters/implementation-text-editor.tex \
	    ../thesis/chapters/introduction.tex \
	    ../thesis/chapters/new-regexes.tex \
	    ../thesis/chapters/notation.tex \
	    ../thesis/chapters/pattern-matching.tex \
	    ../thesis/chapters/regex-and-text-editors.tex \
	    ../thesis/chapters/text-editors-external.tex \
	    ../thesis/chapters/text-editors-internal.tex \
	    ../thesis/chapters/the-real-world.tex \
	    ../thesis/chapters/urls.tex \
	    ../thesis/figures/automatons.tex

CONSTRUCTION_IMAGES = \
		      ../thesis/figures/construction/construction.1 \
		      ../thesis/figures/construction/construction.2 \
		      ../thesis/figures/construction/construction.3 \
		      ../thesis/figures/construction/construction.4 \
		      ../thesis/figures/construction/construction.5 \
		      ../thesis/figures/construction/construction.6 \
		      ../thesis/figures/construction/construction.7 \
		      ../thesis/figures/construction/construction.8 \
		      ../thesis/figures/construction/construction.9 \
		      ../thesis/figures/construction/construction.10 \
		      ../thesis/figures/construction/construction.11 \
		      ../thesis/figures/construction/construction.12 \
		      ../thesis/figures/construction/construction.13 \
		      ../thesis/figures/construction/construction.14 \
		      ../thesis/figures/construction/construction.15 \
		      ../thesis/figures/construction/construction.16 \
		      ../thesis/figures/construction/construction.17 \
		      ../thesis/figures/construction/construction.18 \
		      ../thesis/figures/construction/construction.19

DEFINITIONS_IMAGES = \
		     ../thesis/figures/definitions/definitions.1 \
		     ../thesis/figures/definitions/definitions.2 \
		     ../thesis/figures/definitions/definitions.3 \
		     ../thesis/figures/definitions/definitions.4 \
		     ../thesis/figures/definitions/definitions.5 \
		     ../thesis/figures/definitions/definitions.6 \
		     ../thesis/figures/definitions/definitions.7 \
		     ../thesis/figures/definitions/definitions.8 \
		     ../thesis/figures/definitions/definitions.9 \
		     ../thesis/figures/definitions/definitions.10 \
		     ../thesis/figures/definitions/definitions.11

INTERNAL_IMAGES = \
		  ../thesis/figures/internal/internal.1 \
		  ../thesis/figures/internal/internal.2 \
		  ../thesis/figures/internal/internal.3 \
		  ../thesis/figures/internal/internal.4 \
		  ../thesis/figures/internal/internal.5 \
		  ../thesis/figures/internal/internal.6 \
		  ../thesis/figures/internal/internal.7 \
		  ../thesis/figures/internal/internal.8 \

SOURCE_IMAGES = \
		../thesis/figures/source/piecetree.1 \
		../thesis/figures/source/piecetree.2

PATTERNMATCHER_IMAGES = \
			../thesis/figures/source/patternmatcher.1 \
			../thesis/figures/source/patternmatcher.2 \
			../thesis/figures/source/patternmatcher.3

JPG_IMAGES = \
	     ../thesis/figures/internal/ken-and-dennis-at-a-pdp-11.jpg \

PNG_IMAGES = \
	     ../thesis/figures/internal/ergonomic-keyboard.png \
	     ../thesis/figures/internal/user-input-prompt.png \
	     ../thesis/figures/internal/mouse-left.png \
	     ../thesis/figures/internal/mouse-right.png \
	     ../thesis/figures/internal/mouse-top.png \
	     ../thesis/figures/vim/addressing-1.png \
	     ../thesis/figures/vim/addressing-2.png \
	     ../thesis/figures/vim/delete-1.png \
	     ../thesis/figures/vim/insert-0.png \
	     ../thesis/figures/vim/insert-1.png \
	     ../thesis/figures/vim/insert-2.png \
	     ../thesis/figures/vim/insert-3.png \
	     ../thesis/figures/vim/move-1.png \
	     ../thesis/figures/vim/move-2.png \
	     ../thesis/figures/vim/movement-1.png \
	     ../thesis/figures/vim/movement-2.png \
	     ../thesis/figures/vim/repeating-1.png \
	     ../thesis/figures/vim/repeating-2.png \
	     ../thesis/figures/vim/search-1.png \
	     ../thesis/figures/vim/search-2.png \
	     ../thesis/figures/vim/substitution-1.png \
	     ../thesis/figures/vim/substitution-2.png \
	     ../thesis/figures/vim/start-up.png

IMAGE_FILES = \
	      $(CONSTRUCTION_IMAGES) \
	      $(DEFINITIONS_IMAGES) \
	      $(INTERNAL_IMAGES) \
	      $(SOURCE_IMAGES) \
	      $(PATTERNMATCHER_IMAGES) \
	      $(JPG_IMAGES) \
	      $(PNG_IMAGES)

TXT_FILES = \
	    ../thesis/data/annual-report.txt \
	    ../thesis/data/brown-commentary.txt \
	    ../thesis/data/spelling.txt


.PHONY: all mostly-clean clean


all: masters-project.pdf

masters-project.pdf: $(INC_FILES) $(TEX_FILES) $(IMAGE_FILES) $(TXT_FILES)
	texexec --silent --path=.. masters-project || (rm $@; false)

mostly-clean:
	texutil --purge
	cd ../thesis/figures/construction && texexec --purge
	cd ../thesis/figures/definitions && texexec --purge
	cd ../thesis/figures/internal && texexec --purge

clean:
	texutil --purgeall
	cd ../thesis/figures/construction && texexec --purgeall
	cd ../thesis/figures/definitions && texexec --purgeall
	cd ../thesis/figures/internal && texexec --purgeall

$(CONSTRUCTION_IMAGES): ../thesis/figures/construction/construction.mp
	cd $(dir $<) && texexec --mptex $(notdir $<)

$(DEFINITIONS_IMAGES): ../thesis/figures/definitions/definitions.mp
	cd $(dir $<) && texexec --mptex $(notdir $<)

$(INTERNAL_IMAGES): ../thesis/figures/internal/internal.mp
	cd $(dir $<) && texexec --mptex $(notdir $<)

$(SOURCE_IMAGES): ../thesis/figures/source/piecetree.mp
	cd $(dir $<) && texexec --mptex $(notdir $<)

$(PATTERNMATCHER_IMAGES): ../thesis/figures/source/patternmatcher.mp
	cd $(dir $<) && texexec --mptex $(notdir $<)
