# contents: Ned Library Module.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \section{Initialization}
#
# Most of what’s going on during the initialization step of the editor is to
# \Ruby{require} the necessary modules and set up a dependency||injection
# registry.  Dependency injection is described in the following section.

# ¶ We begin by requring the ruby packaging framework {\em rubygems}
# \cite[Thomas04].  Rubygems will soon be incorporated into Ruby itself, so
# there will be no need to require it explicitly, but until then we’ll have to
# make sure that it gets loaded.
require 'rubygems'

# ¶ Now we need to load our pattern||matcher library and the
# dependency||injection registry class.  It will be up to the client code,
# i.e., the editor driver, to require the rest of the necessary libraries
# through the registry after this file has been loaded.
require 'ned/patternmatcher'
require 'ned/registry'

# ¶ We also set up a service library for \NED:
module Ned
  def self.register_services(container)
    container.define! do
      regexes { Hash.new }
    end
  end

  Needle.register_library 'ned', self
end
