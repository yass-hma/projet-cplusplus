#include "func.h"

using namespace tinyxml2;
using namespace std;

bool test =true;
bool error = false;
bool att = true;

void reload_svg() {
  XMLPrinter printer;
  //svg_data.SaveFile(file);
  svg_data.Print(&printer);
  rsvg_handle_close(svg_handle, NULL);
  svg_handle = rsvg_handle_new_from_data ((const unsigned char*) printer.CStr(), printer.CStrSize(), NULL);
  rsvg_handle_get_geometry_for_element  (svg_handle, NULL, &viewport, NULL, NULL);
}

void *udp_listener(void* )
{
	// https://learn.microsoft.com/en-us/windows/win32/winsock/sockaddr-2
  int sockfd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len;
  char buffer[1024];

  // Create socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  // ------------------------------- Bind socket to address -----------------------------------//
  
  bzero(&server_addr, sizeof(server_addr)); // supprimer les n byte commmence de le pointeur 
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); // Use localhost
  bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)); // liasion de socket à @ip
  
  //-------------------------------------------------------------------------------------------//

  // Receive data in a loop
  while (1) {
    bzero(buffer, sizeof(buffer)); // supprimer les n byte commmence de le pointeur 
    client_len = sizeof(client_addr);
    ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_len);
    cbor_load_result result;
    cbor_root = cbor_load(cbor_data(buffer),n,&result);
     if (cbor_isa_map(cbor_root)){
            getdriven(svg,cbor_root);
            reload_svg();
    		gtk_widget_queue_draw(darea);
  	    }
        cbor_decref(&cbor_root);
        }
  // Close socket
  close(sockfd);
}

template<typename T>
T evaluer_comp_fon(const std::string& expression, cbor_item_t* map) {
	std::string fonction;
	std::vector<T> values;
	std::vector<std::string> keys;
	size_t j = 0;
	for(char c : expression){
		if(c == '('){
			while(expression[j] != '('){
				fonction += expression[j];
				++j;
			}
			j++;
		}else if(c == ',' || c == ')'){
			std::string key;
			while(expression[j] != ',' && expression[j] != ')' ){
				key += expression[j];
				++j;
			}
			j++;
			keys.push_back(key);	
		}else if(c == ' '){
			continue;
		}
	}
	for(const auto& s : keys){
	    size_t index = ft_serach_cle(map,(std::string)s);
            cbor_pair pair= cbor_map_handle(map)[index];
            cbor_item_t* value = pair.value;
            if (keys.empty()) {
                cout << "keys vide" <<endl ;
                error = true;
                return -1;
            }
            if (pair.key == nullptr) {
                cout << "Clé introuvable dans la map CBOR" <<endl ;
                error = true;
                return -1;
            }

            // extraire la valeur de la clé en fonction de son type
            switch (cbor_typeof(value)) {
                case CBOR_TYPE_UINT:
                    values.push_back((T)cbor_get_int(value));
                    break;
                case CBOR_TYPE_NEGINT:
                    values.push_back((T)cbor_get_int(value));
                    break;
                case CBOR_TYPE_FLOAT_CTRL:
                    values.push_back((T)cbor_float_get_float(value));
                    break;
                default:
                    cout << "Type de valeur non reconnu dans la map CBOR" <<endl ;
                    error = true;
                    return -1;
                    break;
            }
	}
	T m = values[0];
	for(size_t i = 1; i < values.size(); ++i){
	    const auto& v = values[i];
            if (values.empty()) {
                cout << "values vide" <<endl ;
                error = true;
                return -1;
            }
            if (v == NULL) {
                cout << "valeur introuvable dans la map CBOR" <<endl ;
                error = true;
                return -1;
            }
            // extraire la valeur de la clé en fonction de son type
            if(fonction == "max") {
                    m = std::max(m,v);
            }else if(fonction == "min"){
                    m = std::min(m,v);
            }else{
                cout << "Type de valeur non reconnu dans la map CBOR" <<endl ;
                error = true;
                return -1;
            }
    }
	if(fonction == "sign"){
                    if(m<0){
                    	return -1;
                    }else{
                        return 1;
                    }
            }else if(fonction == "abs"){
            	    if(m<0){
                    	return -m;
                    }
                    
            }
	return m;
}


template<typename T> 
T evaluate_expression_cbor(const std::string& expression, cbor_item_t* map) {
                        
    std::stack<T> operands;
    std::stack<char> operators;

    for (size_t i = 0; i < expression.size(); ++i) {
        if (isdigit(expression[i])){
            // lire l'opérande
            T operand = 0;
            while (i < expression.size() && isdigit(expression[i])) {
                operand = operand * 10 + (expression[i] - '0');
                ++i;
            }
            operands.push(operand);
            --i;
        }else if (isalpha(expression[i]) || expression[i] == '_' || expression[i] == '&' || expression[i] == '#' || expression[i] == '@' || expression[i] == '$') {
            // lire l'opérande
            std::string key;
            while (i < expression.size() && (isalpha(expression[i]) || expression[i] == '_' || expression[i] == '&' || expression[i] == '#' || expression[i] == '@' || expression[i] == '$')) {
                key += expression[i];
                ++i;
            }
            --i;
            // récupérer la valeur de la clé dans la map CBOR
            size_t index = ft_serach_cle(map,(std::string)key);
            if (index == -1) {
                cout << "Clé introuvable dans la map CBOR" <<endl ;
                error = true;
                return -1;
            }
            cbor_pair pair= cbor_map_handle(map)[index];
            cbor_item_t* value = pair.value;
            // extraire la valeur de la clé en fonction de son type
            switch (cbor_typeof(value)) {
                case CBOR_TYPE_UINT:
                    operands.push((T)cbor_get_int(value));
                    break;
                case CBOR_TYPE_NEGINT:
                    operands.push((T)cbor_get_int(value));
                    break;
                case CBOR_TYPE_FLOAT_CTRL:
                    operands.push((T)cbor_float_get_float(value));
                    break;
                default:
                    cout << "Type de valeur non reconnu dans la map CBOR" << endl;
                    error = true;
                    return -1;
                    break;
            }
        } else if (expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/' || expression[i] == '%') {
            // lire l'opérateur
            int precedence = 0;
            if (expression[i] == '*' || expression[i] == '/') {
                precedence = 1;
            }
	while (!operators.empty() && operators.top() != '(' && ((precedence == 1 && (operators.top() == '*' || operators.top() == '/')) || (precedence == 0 && (operators.top() == '+' ||  operators.top() == '-')))) {
                T y = operands.top(); operands.pop();
                T x = operands.top(); operands.pop();
                char op = operators.top(); operators.pop();
                T result;
                if (op == '+') {
                    result = x + y;
                } else if (op == '-') {
                    result = x - y;
                } else if (op == '*') {
                    result = x * y;
                } else if (op == '/') {
                    if (y == 0) {
                        cout << "Division par zéro"<< endl;
                        error = true;
                        return -1;
                    }
                    result = x / y;
                } else if (op == '%') {
                    if (y == 0) {
                        cout << "Modulo par zéro" << endl;
                        error = true;
                        return -1;
                    }
                    //result = x % y;
                }
                operands.push(result);
            }
            operators.push(expression[i]);
        }else if (expression[i] == '(') {
            operators.push(expression[i]);
        } else if (expression[i] == ')') {
            while (!operators.empty() && operators.top() != '(') {
                T y = operands.top(); operands.pop();
		        T x = operands.top(); operands.pop();
		        char op = operators.top(); operators.pop();
		        T result;
			    if (op == '+') {
                    result = x + y;
                } 
			    else if (op == '-') {
				    result = x - y;
		        }  
			    else if (op == '*') {
				    result = x * y;
		        } 	
			    else if (op == '/') {
				    if (y == 0) {
					    cout << "Division par zéro"<< endl;
                        error = true;
                        return -1;
                    }
				result = x / y;
                } 	
			else if (op == '%') {
                if (y == 0) {
                    cout << "Modulo par zéro"<< endl;
                    error = true;
                    return -1;
                }
                }
		operands.push(result);
		}
		if (operators.empty()) {
            cout << "Parenthèses mal équilibrées"<< endl;
            error = true;
            return -1;
		}
		operators.pop(); // pop la parenthèse ouvrante
	}else if(expression[i] == ' '){
		continue;
	}  
	    }
// check for remaining operators
while (!operators.empty() && (operators.top() == '+' || operators.top() == '-' || operators.top() == '*' || operators.top() == '/' || operators.top() == '%')) {
	if (operators.top() == '(') {
        cout << "Parenthèses mal équilibrées"<< endl;
        error = true;
        return -1;
    }
    T y = operands.top(); operands.pop();
    T x = operands.top(); operands.pop();
    char op = operators.top(); operators.pop();
    T result;
    if (op == '+') {
        result = x + y;
    } else if (op == '-') {
        result = x - y;
    } else if (op == '*') {
        result = x * y;
    } else if (op == '/') {
        if (y == 0) {
            cout << "Divise par zéro"<< endl;
            error = true;
            return -1;
        }
        result = x / y;
    } else if (op == '%') {
        if (y == 0) {
            cout << "Modulo par zéro"<< endl;
            error = true;
            return -1;
        }
    }
    operands.push(result);
}
return operands.top();  
}


template<typename T>
T evaluer_geo_fon(const std::string& expression, cbor_item_t* map) {
	std::string fonction;
	std::string key;
	T result;
	size_t j = 0;
	for(char c : expression){
		if(c == '('){
			while(expression[j] != '('){
				fonction += expression[j];
				++j;
			}
			j++;
		}else if(c == ')'){
			while(expression[j] != ')' ){
				key += expression[j];
				++j;
			}	
		}else if(c == ' '){
			continue;
		}
	}
	if(isarethfunction(key)){
		result = evaluate_expression_cbor<T>(key,map);
	}else if(isgeofunction(key)){
		result = evaluer_geo_fon<T>(key, map);
	}else if(iscompfunction(key)){
		result = evaluer_comp_fon<T>(key, map);
	}else{
	    size_t index = ft_serach_cle(map,(std::string)key);
            cbor_pair pair= cbor_map_handle(map)[index];
            cbor_item_t* value = pair.value;
            if (index == -1) {
		        cout << "Clé introuvable dans la map CBOR "<< endl;
		        error = true;
		        return -1;
            }

            // extraire la valeur de la clé en fonction de son type
            switch (cbor_typeof(value)) {
                case CBOR_TYPE_UINT:
                    result = (T)cbor_get_int(value);
                    break;
                case CBOR_TYPE_NEGINT:
                    result = (T)cbor_get_int(value);
                    break;
                case CBOR_TYPE_FLOAT_CTRL:
                    result = (T)cbor_float_get_float(value);
                    break;
                default:
                	cout << "Type de valeur non reconnu dans la map CBOR"<< endl;
            		error = true;
           			return -1;
                    break;
            }
	}
	
	if(fonction == "log"){
		if(result > 0){
			return std::log(result);
		}else{
			cout << "invalide valeur pour log"<< endl;
            error = true;
            return -1;
		}
	}else if(fonction == "cos"){
		return std::cos(result);
	}else if(fonction == "sin"){
		return std::sin(result);
	}else if(fonction == "tan"){
		return std::tan(result);
	}else{
			cout << "invalide valeur pour log"<< endl;
            error = true;
            return -1;
	}
}

bool checkAttributein(std::string attribute) {
    if (attribute == "opacity"|| attribute == "display" || attribute == "fill-rule" || attribute == "font-style" || attribute == "text-anchor" || attribute == "baseline-shift" || attribute == "vector-effect" || attribute == "shape-rendering" || attribute == "image-rendering" || attribute == "pointer-events") {
        return true;
    } else {
        return false;
    }
}

bool checkAttributeValue(std::string attribute, std::string value) {
    if (attribute == "opacity") {
        return (std::stof(value) <= 1 && std::stof(value) >= 0);
    } else if (attribute == "display") {
        return (value == "none" || value == "inline");
    } else if (attribute == "fill-rule") {
        return (value == "evenodd" || value == "nonzero");
    } else if (attribute == "font-style") {
        return (value == "normal" || value == "italic" || value == "oblique");
    } else if (attribute == "text-anchor") {
        return (value == "start" || value == "middle" || value == "end");
    } else if (attribute == "baseline-shift") {
        return (value == "baseline" || value == "sub" || value == "super");
    } else if (attribute == "vector-effect") {
        return (value == "none" || value == "non-scaling-stroke" || value == "non-scaling-size" || value == "non-rotation" || value == "fixed-position" || value == "stroke-width" || value == "stroke-first");
    } else if (attribute == "shape-rendering") {
        return (value == "auto" || value == "optimizeSpeed" || value == "crispEdges" || value == "geometricPrecision");
    } else if (attribute == "image-rendering") {
        return (value == "auto" || value == "optimizeSpeed" || value == "optimizeQuality" || value == "pixelated");
    } else if (attribute == "pointer-events") {
        return (value == "none" || value == "visiblePainted" || value == "visibleFill" || value == "visibleStroke" || value == "visible" || value == "painted" || value == "fill" || value == "stroke"  || value == "all");
    } else {
        // Attribute not found
        return false;
    }
}

bool iscompfunction(std::string func){
	bool isfunc = false;
	std::string fonction;
	size_t j = 0;
	for(char c : func){
		if(c == '('){
			while(func[j] != '('){
				fonction += func[j];
				++j;
			}
		}
	}
	if(fonction == "max" || fonction == "min" || fonction == "abs" || fonction == "sign"){
		isfunc = true;
	}
	return isfunc;
}

bool isgeofunction(std::string func){
	bool isfunc = false;
	std::string fonction;
	size_t j = 0;
	for(char c : func){
		if(c == '('){
			while(func[j] != '('){
				fonction += func[j];
				++j;
			}
		}
	}
	if(fonction == "log" || fonction == "cos" || fonction == "sin" || fonction == "tan"){
		isfunc = true;
	}
	return isfunc;
}

bool islengthfunction(std::string func){
	bool isfunc = false;
	std::string fonction;
	size_t j = 0;
	for(char c : func){
		if(c == '('){
			while(func[j] != '('){
				fonction += func[j];
				++j;
			}
		}
	}
	if(fonction == "length"){
		isfunc = true;
	}
	return isfunc;
}

bool isarethfunction(std::string func){
	bool isfunc = false;
	for(char c : func){
        if(c == '+' || c == '*' || c == '-' || c == '/' || c == '%' || func[0] == '('){
            isfunc = true;
        }
	}
	return isfunc;
}

int evaluer_longueur(const std::string& expression, cbor_item_t* map){
	std::string fonction;
	std::string key;
	size_t j = 0;
	for(char c : expression){
		if(c == '('){
			while(expression[j] != '('){
				fonction += expression[j];
				++j;
			}
			j++;
		}else if(c == ')'){
			while(expression[j] != ')' ){
				key += expression[j];
				++j;
			}	
		}
	}
	size_t index = ft_serach_cle(map,(std::string)key);
            cbor_pair pair= cbor_map_handle(map)[index];
            cbor_item_t* value = pair.value;
            if (pair.key == nullptr) {
                throw std::invalid_argument("Clé introuvable dans la map CBOR");
            }
            if (!cbor_isa_string(pair.key)) {
            	throw std::invalid_argument("valeur de la Clé n'est pas string");
                }
               return  cbor_string_length(value);
}

bool isinstyle(std::string stl, std::string att){
	bool in = false;
	std::string ch = att;
	if(stl.find(ch) != std::string::npos){
		in = true;
	}
	return in;
}

// function search d'index od by
size_t ft_serach_cle(cbor_item_t *cbor_root,std::string by){
    size_t map_size = cbor_map_size(cbor_root);
    for (size_t i = 0; i < map_size; i++){
        cbor_pair pair = cbor_map_handle(cbor_root)[i];
        std::string key_str = std::string((char*)cbor_string_handle(pair.key), cbor_string_length(pair.key));
            if (key_str == by) {
                return i;
            }
        }
        return -1;
}


void* update_element_verification(XMLElement* parent,const char *target, const char *targetType,const char *by, cbor_item_t *cbor_root)
{
	void* res;
	if(isarethfunction(by)){
		if(strcmp(targetType,"float") == 0){
			float result = evaluate_expression_cbor<float>(by,cbor_root);
			if (result == -1 && error){
				res = (void *)"err";
				return res;
			}
			if((checkAttributein(target) && checkAttributeValue(target,std::to_string(result))) || (!checkAttributein(target)))
				res = &result;
			else{
				error = true;
				cout << "WARNING : "<< target <<" accept unique values" << endl;
				res = (void *)"err";
			}
		}else if(strcmp(targetType,"int") == 0){
			int result = evaluate_expression_cbor<int>(by,cbor_root);
			if (result == -1 && error){
		       		res = (void *)"err";
				return res;
			}
			if((checkAttributein(target) && checkAttributeValue(target,std::to_string(result))) || (!checkAttributein(target)))
				res = &result;
			else{
				error = true;
				cout << "WARNING : "<< target <<" accept unique values" << endl;
				res = (void *)"err";
			}
			}else{
				error = true;
				cout << "WARNING : "<< target <<" and by haven't the same type " << endl;
				res = (void *)"err";
			}
      	}else if(iscompfunction(by)){
            	if(strcmp(targetType,"float") == 0){
			float result = evaluer_comp_fon<float>(by,cbor_root);
		        if (result == -1 && error){
		            res = (void *)"err";
		            return res;
		        }
		        if((checkAttributein(target) && checkAttributeValue(target,std::to_string(result))) || (!checkAttributein(target)))
				res = &result;
			else{
				error = true;
				cout << "WARNING : "<< target <<" accept unique values" << endl;
				res = (void *)"err";
			}
            	}else if(strcmp(targetType,"float") == 0){
		        int result = evaluer_comp_fon<int>(by,cbor_root);
		        if (result == -1 && !error){
				res = (void *)"err";
				return res;
		        }
			if((checkAttributein(target) && checkAttributeValue(target,std::to_string(result))) || (!checkAttributein(target)))
				res = &result;
			else{
				error = true;
				cout << "WARNING : "<< target <<" accept unique values" << endl;
				res = (void *)"err";
			}
		}else{
			error = true;
			cout << "WARNING : "<< target <<" and by haven't the same type " << endl;
			res = (void *)"err";
		}
      	}else if(isgeofunction(by)){
            	if(strcmp(targetType,"float") == 0){
			float result = evaluer_geo_fon<float>(by,cbor_root);
			if (result == -1 && !error){
				res = (void *)"err";
				return res;
			}
			res = &result;
		}else if(strcmp(targetType,"int") == 0){
			int result = evaluer_geo_fon<int>(by,cbor_root);
			if (result == -1 && !error){
				res = (void *)"err";
				return res;
			}
			if((checkAttributein(target) && checkAttributeValue(target,std::to_string(result))) || (!checkAttributein(target)))
				res = &result;
                	else{
		                error = true;
		                cout << "WARNING : "<< target <<" accept unique values" << endl;
		                res = (void *)"err";
                    }
		}else{
			error = true;
			cout << "WARNING : "<< target <<" and by haven't the same type " << endl;
			res = (void *)"err";
		}
      	}else if(islengthfunction(by)){
      		int r = evaluer_longueur(by,cbor_root);
      		std::cout << r <<std::endl;
      		res = &r;
      	}else{
      		size_t index = ft_serach_cle(cbor_root,(std::string)by);
      		if (index != -1){
      			test = true;
      			cbor_pair pair= cbor_map_handle(cbor_root)[index];
               		cbor_item_t* value = pair.value;
      			switch (cbor_typeof(value)) {
		        	case CBOR_TYPE_UINT:
		        	    if(strcmp(targetType,"int") == 0){
		        	    	int result = cbor_get_int(value);
		        	  		if((checkAttributein(target) && checkAttributeValue(target,std::to_string(result))) || (!checkAttributein(target)))
							res = &result;
			    	    		else{
							error = true;
							cout << "WARNING : "<< target <<" accept unique values" << endl;
							res = (void *)"err";
						}
		            	     }else{
						error = true;
						cout << "WARNING : "<< target <<" and by haven't the same type " << endl;
						res = (void *)"err";
					}
		            		break;
		        	 case CBOR_TYPE_FLOAT_CTRL:
				    	if(strcmp(targetType,"float") == 0){
				    			
			    			float result = 	cbor_float_get_float(value);
			    			
			    			if((checkAttributein(target) && checkAttributeValue(target,std::to_string(result))) || (!checkAttributein(target)))
							res = &result;
							
						else{
							error = true;
							cout << "WARNING : "<< target <<" accept unique values" << endl;
							res = (void *)"err";
							}     
				    	}else{
						error = true;
						cout << "WARNING : "<< target <<" and by haven't the same type " << endl;
						res = (void *)"err";
							}
		           		break;
		        	case CBOR_TYPE_STRING:
		           		if(strcmp(targetType,"string") == 0){	
			  			std::string* v = new std::string(std::string((char*)cbor_string_handle(value), cbor_string_length(value)));
			  			res = v;
		            		}else{
		            			error = true ;
						cout << "WARNING : "<< target <<" and by haven't the same type " << endl;
		         			res = (void* )"err";
		            		}
		            		break;
		            	case CBOR_TYPE_BYTESTRING:
		           		if(strcmp(targetType,"color") == 0){
						uint8_t *hex_value = cbor_bytestring_handle(value);
						size_t hex_value_size = cbor_bytestring_length(value);
					    	char hex_string[hex_value_size*2 +1];
					    	sprintf(hex_string, "#");
						for (int i = 0; i < hex_value_size; i++) {
							sprintf(hex_string + i*2 + 1, "%02x", hex_value[i]);
						}						    
					  	std::string* v = new std::string(std::string((char*)hex_string, sizeof(hex_string)));
					  	res = v;
		            		}else{
				    		error = true;
				    		std::cout << "error target" << target << endl;
		            		}
		            		break;
		        	default:
					error = true ;
			    		cout << "WARNING : Type de valeur non reconnu dans la map CBOR" << endl;
			 		res = (void* )"err";
		            		break;
		    	}
		    }else{
		    	test = false;
		    	error = true ;
		    	res = (void* )"err";
		    	cout << "by : " << by << " , n'existe pas dans la map CBOOR" << endl;
		    	cout << (char*)res << endl;
		    }
		  }
     return res; 	
}

void animate_element_attribute_change(XMLElement* parent, const char* target, float start_value, float end_value, int duration_ms) {

	 const int steps = 5;
    const float delay_factor = 1.0f / steps;

    float current_value = start_value;
    float increment = (end_value - start_value) / steps;
    int elapsed_time_ms = 0;
    int step_duration_ms = duration_ms / steps;

    auto start_time = std::chrono::steady_clock::now();
    auto end_time = start_time + std::chrono::milliseconds(duration_ms);

    while (std::chrono::steady_clock::now() < end_time) {
        current_value += increment;
        parent->SetAttribute(target, current_value);
        reload_svg();
        gtk_widget_queue_draw(darea);
        std::this_thread::sleep_for(std::chrono::milliseconds(step_duration_ms));
        elapsed_time_ms += step_duration_ms;
    }

    // Animation is finished, set the attribute to the final value
    parent->SetAttribute(target, end_value);
    reload_svg();
    gtk_widget_queue_draw(darea);
/*
	const int steps =15;  
	const float delay_factor = 1.0f / steps;  

	float current_value = start_value;
	float increment = (end_value - start_value) / steps;

	for (int i = 0; i < steps; i++) {
		current_value += increment;

		parent->SetAttribute(target, current_value);

		reload_svg();
		gtk_widget_queue_draw(darea);
		usleep(duration_ms*100);
	}
	parent->SetAttribute(target, end_value);*/
}

// Fonction récursive pour chercher l'élément driven et récupérer ses attributs et verifie le type de target et verifie si target est un attribut direct ou si e
void getdriven(XMLElement* parent, cbor_item_t *cbor_root){
  if (!parent) return;  // Vérification de sécurité pour s'assurer que le parent existe , cas de base

  XMLElement* child = parent->FirstChildElement();
  while (child) {
    if (std::string(child->Name()) == "driven") {  // Vérification si l'élément est "driven"
      const char* target = child->Attribute("target");
      const char* by = child->Attribute("by");
      const char* style = parent->Attribute("style");
      const char* verifie_target = parent->Attribute(target);
      const char* targetType = child->Attribute("type");
      int delay = child->IntAttribute("delay");
      
      if (verifie_target){
      		update_element_verification(parent,target,targetType,by,cbor_root);
      if(error){
    		error = false;
      }else{
      	if(strcmp(targetType,"float") == 0){
      			float res = parent->FloatAttribute(target);
		  	if (test){
			  	float resultat_anlyze = *((float*)update_element_verification(parent,target,targetType,by,cbor_root));
		      			animate_element_attribute_change(parent,target, res, resultat_anlyze, delay);
		      		
		  	}   
	}else if(strcmp(targetType,"string") == 0){
			if (test){
				std::string resultat_anlyze = *((std::string *)update_element_verification(parent,target,targetType,by,cbor_root));
				parent->SetAttribute(target,resultat_anlyze.c_str());
			}
      	}else if(strcmp(targetType,"color") == 0){
	      		if (test){
		      		std::string resultat_anlyze = *((std::string *)update_element_verification(parent,target,targetType,by,cbor_root));
		      		parent->SetAttribute(target,resultat_anlyze.c_str());
		      	}
      	}else{
	      		if (test){
	      	      		int resultat_anlyze = *((int *)update_element_verification(parent,target,targetType,by,cbor_root));
		      		parent->SetAttribute(target,resultat_anlyze);
		      		
	      		}
      	}
      }
     }else if (style && isinstyle(style,target)){
      		std::string style_string = target;
      		if(strcmp(targetType,"float") == 0){
      	      		float resultat_anlyze = *((float*)update_element_verification(parent,target,targetType,by,cbor_root));
      	      		style_string += ':'+std::to_string(resultat_anlyze);
      	      		std::cout << style_string << endl;
	      	}
	      	else if(strcmp(targetType,"string") == 0){
	      		std::string resultat_anlyze = *((std::string *)update_element_verification(parent,target,targetType,by,cbor_root));
	      		style_string += ':'+resultat_anlyze;
	      		std::cout << style_string << endl;
	      	}else if(strcmp(targetType,"color") == 0){
	      		std::string resultat_anlyze = *((std::string *)update_element_verification(parent,target,targetType,by,cbor_root));
	      		style_string += ':'+resultat_anlyze;
	      		std::cout << style_string << endl;
	      	}
	      	else{
	      		int resultat_anlyze = *((int *)update_element_verification(parent,target,targetType,by,cbor_root));
	      		style_string += ':'+std::to_string(resultat_anlyze);
	      		std::cout << style_string << endl;
	      	}
      		std::string ch = evaluer_style(style, style_string);
		parent->SetAttribute("style",ch.c_str());
		
      	}
      	else{
      		cout << "target : " << target << " , n'existe pas !!!!" << endl;
 	}
    }
    getdriven(child,cbor_root);  // Recherche récursive dans les enfants
    child = child->NextSiblingElement();
 }
}

// Fonction pour manipuler l'attribut style l'argument style est la chane style d'origine qu'on a et param est sous forme "sous_attribut:value" on cherche le "sous_attribut" dans la chaine style et on remplace sa valeur avec value.
//la fonction retourne une chaine de caractères qui représente le nouveau style qu'on a
string evaluer_style(string style, string param) {
    std::stringstream ss(style);
    std::string item;
    std::vector<string> params;
    
    while (getline(ss, item, ';')) {
        params.push_back(item);
    }
    
    std::string val1 = param.substr(0 ,param.find(":"));
    std::string val2 = param.substr(param.find(":")+1);
    
   for (int i = 0; i < params.size(); i++) {
        std::string par1 = params[i].substr(0 ,param.find(":"));
        std::string par2 = params[i].substr(param.find(":")+1);
        if(par1 == val1){
        	std::string res = par1+":"+val2;
        	params[i] = res;
        	break;
        }
    
    }
    std::string new_style = ""; 
    for (int i = 0; i < params.size(); i++) {
    	if(i == params.size()-1){
    		new_style += params[i];
    	}else{
    		new_style += params[i] + ";";
    	}
    }
    return new_style;
}
