# Overview

Something I furrow my brow over here is that I am unsure where
equivalent representations of the same data should be normalized.

For example, do I want to to send sources via the `id` or via the
`name`? If I want to allow for `orihime add source` to allow either a
number or a string for the fourth argument, should I convert that
immediately while processing `argv` or should I pass the strings along
to the gRPC client and allow it to convert it before setting the
`protobuf` message? Or should the `protobuf` message allow for `oneof`
messages where the source can be represented by either the `id` or the
`number`?
