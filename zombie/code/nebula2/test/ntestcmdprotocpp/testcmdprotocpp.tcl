sel /
new ntestcmdprotocpp test
sel test
    # 0 arguments, no return
    .test0vv
    
    # 0 arguments, return type
    .test0iv
    .test0bv
    .test0fv
    .test0sv
    .test0ov
    .test0ov1

    # 0 arguments, constant return type
    .test0civ
    .test0cbv
    .test0cfv
    .test0csv
    .test0cov
    .test0cov1
    
    # 1 argument, no return
    .test1vi 24
    .test1vb false
    .test1vb true
    .test1vf 12.5
    .test1vs hello
    .test1vo /test
    .test1vo2 /test
    
    # 1 constant argument, no return
    .test1vci 24
    .test1vcb false
    .test1vcb true
    .test1vcf 12.5
    .test1vcs hello
    .test1vco /test
    .test1vco2 /test
    
    # 1 parameter, return type
    .test1ii 20
    .test1bb false
    .test1bb true
    .test1ff 23.0
    .test1ss hello
    .test1oo /test
    .test1oo2 /test
    
    # 1 parameter, constant return type
    .test1cii 20
    .test1cbb false
    .test1cbb true
    .test1cff 23.0
    .test1css hello
    .test1coo /test
    .test1coo2 /test    
    
    # 1 constant parameter, return type
    .test1ici 20
    .test1bcb false
    .test1bcb true
    .test1fcf 23.0
    .test1scs hello
    .test1oco /test
    .test1oco2 /test

    # 1 constant parameter, constant return type
    .test1cici 20
    .test1cbcb false
    .test1cbcb true
    .test1cfcf 23.0
    .test1cscs hello
    .test1coco /test
    .test1coco2 /test    
    
    # multiple arguments, return
    .test6iibfsoo 20 false 23.0 hello /test /test
    .test6iibfsoo 2 true 5.0 hello /test /test
    .test6icicbcfcscoco 20 false 23.0 hello /test /test
    .test6icicbcfcscoco 1 true 3.0 hello /test /test
    .test6vcicbcfcscoco 20 false 23.0 hello /test /test
    .test6vcicbcfcscoco 20 false 23.0 hello /test /test

    # new types
    .test0v3v
    .test0cv3v
    .test0v3pv
    .test0cv3pv
    
    .test1vv3 1.0 2.0 3.0
    .test1vcv3 1.0 2.0 3.0
    .test1vv3p 1.0 2.0 3.0
    .test1vcv3p 1.0 2.0 3.0
    
    # 1 argument, no return
    .reftest1vi 24
    .reftest1vb false
    .reftest1vb true
    .reftest1vf 12.5
    .reftest1vs hello
    .reftest1vo /test
    .reftest1vo2 /test
    
    # 1 parameter, return type
    .reftest1ii 20
    .reftest1bb false
    .reftest1bb true
    .reftest1ff 23.0
    .reftest1ss hello
    .reftest1oo /test
    .reftest1oo2 /test
    