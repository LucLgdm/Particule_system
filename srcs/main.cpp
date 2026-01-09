/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 09:36:26 by lde-merc          #+#    #+#             */
/*   Updated: 2026/01/09 14:01:06 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Application.hpp"

int main(int argc, char **argv) {
	std::cout << "Hello Particule" << std::endl;
	Application app;
	try {
		app.init(argc, argv);
		app.run();
	} catch (inputError &e) {
		std::cerr << "\033[31mInput error:\033[m" << std::endl << e.what() << std::endl;
	} catch (glfwError &e) {
		std::cerr << "\033[31mGLFW error:\033[m" << std::endl << e.what() << std::endl;
	} catch (openGlError &e) {
		std::cerr << "\033[31mOpenGl error:\033[m" << std::endl << e.what() << std::endl;
	} catch (openClError &e) {
		std::cerr << "\033[31mOpenCl error:\033[m" << std::endl << e.what() << std::endl;
	} catch (std::runtime_error &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;	
}