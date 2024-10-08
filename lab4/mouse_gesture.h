#pragman once

enum mouse_ev_t { 
    LB_PRESSED,     
    LB_RELEASED,    
    RB_PRESSED,     
    RB_RELEASED,    
    BUTTON_EV,      
    MOUSE_MOV       
};


static struct mouse_ev {
    enum mouse_ev_t type;       
    int16_t delta_x;            
    int16_t delta_y;            
} m_ev;

struct mouse_ev *mouse_detect_event(struct packet *pp);