$:.unshift File.expand_path("../lib", __FILE__)
require "tuple/version"

Gem::Specification.new do |spec|
  spec.name          = "tuple"
  spec.authors       = ["Justin Balthrop", "Ash Moran", "topac"]
  spec.email         = "code@justinbalthrop.com"
  spec.description   = "Fast, binary-sortable serialization for arrays of simple Ruby types"
  spec.summary       = "Tuple serialization functions"
  spec.homepage      = "http://github.com/ninjudd/tuple"
  spec.version       = Tuple::VERSION
  spec.license       = "MIT"

  spec.files         = `git ls-files`.split($\)

  spec.executables   = spec.files.grep(%r{^bin/}).map{ |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.extensions    = ["ext/tuple/extconf.rb"]
  spec.require_paths = ["lib"]

  if spec.respond_to?(:metadata)
    spec.metadata['allowed_push_host'] = "TODO: Set to 'http://mygemserver.com' to prevent pushes to rubygems.org, or delete to allow pushes to any server."
  end

  spec.add_development_dependency 'rake-compiler', '~> 0.8.3'
  spec.add_development_dependency 'test-unit', '~> 2.0'
end
