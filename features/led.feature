Feature: Hello World

Scenario: Blink red LED
	Given I have a red led
	When I turn it on
	Then it's lit up
