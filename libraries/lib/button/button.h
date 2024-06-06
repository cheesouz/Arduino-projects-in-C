#define BUTTON_PORT PORTC
#define BUTTON_PIN PINC
#define BUTTON_DDR DDRC
#define BUTTON1 PC1
#define BUTTON2 PC2
#define BUTTON3 PC3

void enableOneButton( int button );
void enableAllButtons();
void setupButtons();
int buttonPushed( int button );
int buttonPushedWithDebounce(int button);
int buttonReleased( int button );