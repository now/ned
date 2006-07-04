# contents: Regular expression registry.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \section{The Regular Expression Registry}
#
# As our pattern||matcher library allows us to extend our syntax through the
# use of rules, we provide the user with a way to register such rules with the
# editor so that it can then pass them on to the pattern||matcher during
# initialization:

module Ned
  module Regexes
    def self.register_services(container)
      container.define :regexes do
        Hash.new
      end
    end

    Needle.register_library 'ned', self
  end
end
