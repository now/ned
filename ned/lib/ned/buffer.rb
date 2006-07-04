# contents: Buffer sub-module registry.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \section{Buffers}
#
# Our first encounter with dependency injection is a service library for the
# subsystem that deals with buffers in our text editor.  In needle, a service
# library is defined by a module||method named \Ruby{register_services}.  This
# method takes a registry container and should define a namespace within it.
# This namespace should be unique for the subsystem|/|library and all services
# should be defined within this namespace.  (A namespace is actually a registry
# within a registry.)  The service library that we define only consists of a
# buffer class.

module Ned
  module Buffer
    def self.register_services(container)
      container.namespace_define :buffer do |b|
        b.buffer(:model => :prototype) do |c, p, file|
          require 'ned/buffer/buffer'
          Buffer.new(file)
        end
      end
    end

    Needle.register_library 'ned/buffer', self
  end
end

# ¶ Buffers are prototypes, as we want separate buffers for any arguments we
# are passed.  Note how even loading of the definition of our buffer class is
# deferred to actual access of the buffer service.
