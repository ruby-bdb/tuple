# Tuple

Tuple provides fast, binary-sortable serialization for arrays of simple Ruby types.

This means you do not have to deserialize your tuples to store them. This leads to significant
performance benifits when using Tuples as keys for a BTree.

A Tuple is just an Array of any number of simple Ruby types. The following types are
supported (listed in ascending sort order):

1. NilClass
2. FalseClass
3. Integer (Fixnum or Bignum)
4. String
5. Symbol
6. True

## Usage

```ruby
  require 'tuple'

  data = Tuple.dump([1, -43, :foo, "bar", true, false, nil])
  # => "\000\000\020\000\000\000\000\001..."
  Tuple.load(data)
  # => [1, -43, :foo, "bar", true, false, nil]
```

## Install

```
  gem install tuple
```

## License

Copyright (c) 2009 Justin Balthrop; Published under The MIT License, see LICENSE file.