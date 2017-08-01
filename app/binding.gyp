{
  "targets": [
    {
      "target_name": "usercheck",
      "sources": [ "usercheck.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}