# contents: Evil modifications to base classes.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>

class Class
  def def_access scope, name, &blk
    if [:public, :protected, :private].include? scope
      define_method name, &blk
      self.send scope, name
    else
      raise ArgumentError, "illegal visibility: %s", scope
    end
  end
end
