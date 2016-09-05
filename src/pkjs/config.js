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
// 	{
// 		"type": "section",
// 		"items": [
// 			{
// 				"type": "heading",
// 				"defaultValue": "Battery"
// 			},
// 			{
// 				"type": "toggle",
// 				"messageKey": "TOGGLE_BATTERY",
// 				"label": "Show battery icon",
// 				"defaultValue": false,
// 			},
// 			{
// 				"type": "select",
// 				"messageKey": "SELECT_BATTERY_PERCENT",
// 				"defaultValue": "20",
// 				"label": "Low battery percent",
// 				"options": [
// 					{ 
// 						"label": "10%",
// 						"value": "10" 
// 					},
// 					{ 
// 						"label": "20%",
// 						"value": "20" 
// 					},
// 					{ 
// 						"label": "30%",
// 						"value": "30" 
// 					},
// 					{ 
// 						"label": "40%",
// 						"value": "40" 
// 					},
// 					{ 
// 						"label": "50%",
// 						"value": "50" 
// 					},
// 					{ 
// 						"label": "60%",
// 						"value": "60" 
// 					},
// 				]
// 			},
// 		]
//	},
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
	}
];

