{
	"targets": [
		{
			"target_name": "idlerun",
			"sources": [
				"src/idlerun.cc"
			],
			"include_dirs" : [
				"<!(node -e \"require('nan')\")"
			],
			"conditions": [
				['OS == "win"',
					{'defines': ['WINDOWS']}
				]
			],
		}
	]
}
