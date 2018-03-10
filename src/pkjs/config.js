module.exports = [
	{
		"type": "heading",
		"defaultValue": "Sharingan",
		"size": 1,
	},
	{
		"type": "section",
		"items": [
			{
				"type": "heading",
				"defaultValue": "Colour Selection"
			},
			{
				"type": "color",
				"messageKey": "COLOUR_BLUETOOTH",
				"label": "Blutooth Disconnection",
				"defaultValue": "FF0000",
				"sunlight": true,
				"capabilities": ["NOT_BW"],
			}
		]
	},
	{
		"type": "section",
		"items": [
			{
				"type": "heading",
				"defaultValue": "Digital Time"
			},
			{
				"type": "toggle",
				"messageKey": "TOGGLE_DIGITAL",
				"label": "Toggle Digital time",
				"defaultValue": false,
			}
		]
	},
	{
		"type": "section",
		"items": [
			{
				"type": "heading",
				"defaultValue": "Date"
			},
			{
				"type": "toggle",
				"messageKey": "TOGGLE_SUFFIX",
				"label": "Show suffix",
				"defaultValue": false,
			},
		]
	},
	{
		"type": "submit",
		"defaultValue": "Save"
	},
	{
		"type": "text",
		"defaultValue": " Version 0.6",
	},
];
