#pragma once

#ifndef CORES_H
#define CORES_H

// --- Reset ---
#define RESET       "\033[0m"

// --- Cores de Texto (Regulares) ---
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"

// --- Cores de Texto (Negrito/Bold) ---
#define BOLD_RED    "\033[1;31m"
#define BOLD_GREEN  "\033[1;32m"
#define BOLD_YELLOW "\033[1;33m"
#define BOLD_BLUE   "\033[1;34m"
#define BOLD_CYAN   "\033[1;36m"
#define BOLD_WHITE  "\033[1;37m"

// --- Cores de Fundo (Background) ---
#define BG_RED      "\033[41m"
#define BG_GREEN    "\033[42m"
#define BG_YELLOW   "\033[43m"
#define BG_BLUE     "\033[44m"
#define BG_MAGENTA  "\033[45m"
#define BG_CYAN     "\033[46m"
#define BG_WHITE    "\033[47m"

// --- Estilos Adicionais ---
#define UNDERLINE   "\033[4m"
#define REVERSED    "\033[7m" // Inverte cor do texto e do fundo

#endif