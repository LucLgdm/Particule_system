/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Global.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 11:54:51 by lde-merc          #+#    #+#             */
/*   Updated: 2026/02/19 17:22:28 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

static int HEIGHT = 1200;
static int WIDTH = 1600;

struct KeyState {
    bool isDown = false;
    bool wasDown = false;

    bool pressed() const {
        return isDown && !wasDown;
    }

    bool released() const {
        return !isDown && wasDown;
    }

    void update(bool currentDown) {
        wasDown = isDown;
        isDown = currentDown;
    }
};
