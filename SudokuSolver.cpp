#include <algorithm>
#include <array>
#include <iostream>
#include <vector>
#include <chrono>
//
using digit = char;
constexpr digit lado = 9;
constexpr digit lado2 = lado * lado;
using sudoku = std::array<digit, lado2>;
using aVecDigit81 = std::array<std::vector<digit>, lado2>;
using aDigit9 = std::array<digit, lado>;
using numeros = std::vector<digit>;
constexpr digit hole = 0;
//
enum class ESolucion
{
   Ok,
   Imposible,
   No_avanzo,
   Avanzo,
};
//
void PrintSudoku(const sudoku &s, digit resaltarIndice = -1)
{
   digit numCeros = 0;
   // Recorrer filas
   for(digit f = 0; f < lado; f++)
   {
      // Recorrer columnas
      for(digit c = 0; c < lado; c++)
      {
         // Obtener índice
         const auto i = f * lado + c;
         if(i == resaltarIndice)
            std::cout << 1 * s[i] << ']'; // Multiplico para convertir a entero
         else
            std::cout << 1 * s[i] << ' '; // Multiplico para convertir a entero

         if(hole == s[i])
            ++numCeros;
      }
      std::cout << '\n';
   }
   std::cout << '\n';
   std::cout << 1 * numCeros << " celdas pendientes" << '\n';
}


void UpdateSudoku(sudoku &s, digit i, digit value, bool bPrint = false)
{
   s[i] = value;
   /*if(bPrint)
   {
      std::cout << '\n';
      PrintSudoku(s, i);
      std::cout << '\n';
   }*/

}


void Print(const numeros &nums)
{
   for(const auto &num : nums)
   {
      std::cout << num << " ";
   }
   std::cout << '\n';
}

constexpr digit InicioBloque(digit bloque)
{
   const digit inicio[] = { 0, 3, 6, 27, 30, 33, 54, 57, 60 };
   return inicio[bloque];
}


constexpr aDigit9 ObtenerIndicesFila(digit fila)
{
   const digit delta = 9 * fila;
   aDigit9 indices = { delta, digit(1) + delta, 2 + delta, 3 + delta, 4 + delta, 5 + delta, 6 + delta, 7 + delta, 8 + delta };
   return indices;
}


constexpr aDigit9 ObtenerIndicesColumna(digit col)
{
   aDigit9 indices = { 0 + col, 9 + col, 18 + col, 27 + col, 36 + col, 45 + col, 54 + col, 63 + col, 72 + col };
   return indices;
}


constexpr aDigit9 ObtenerIndicesBloque(digit bloque)
{
   const digit iBloque = InicioBloque(bloque);
   aDigit9 indices = { iBloque, 1 + iBloque, 2 + iBloque, 9 + iBloque, 10 + iBloque, 11 + iBloque, 18 + iBloque, 19 + iBloque, 20 + iBloque };

   return indices;
}


numeros NumerosPosibles(const sudoku &s, digit i)
{
   const digit f = i / lado;
   const digit c = i - 9 * f;
   const digit bloque = 3 * (f / 3) + (c / 3);

   // Si el nº1 ya existe el elemento [0] es no nulo.
   aDigit9 yaexistentes = { 0 };

   // ¿Qué nº ya están en la fila?
   for(digit k = 0; k < lado; ++k)
   {
      const auto posicion = f * lado + k;
      if(hole != s[posicion])
         yaexistentes[s[posicion] - 1] = 1;
   }
   //Print(yaexistentes);

   // ¿Qué nº ya están en la columna?
   for(digit k = 0; k < lado; ++k)
   {
      const auto posicion = c + k * lado;
      if(hole != s[posicion])
         yaexistentes[s[posicion] - 1] = 1;
   }
   //Print(yaexistentes);

   // ¿Qué nº ya están en el bloque?
   auto indices = ObtenerIndicesBloque(bloque);
   for(auto &idx : indices)
   {
      if(hole != s[idx])
         yaexistentes[s[idx] - 1] = 1;
   }
   //Print(yaexistentes);

   // Los posibles son los que faltan en los ya existentes
   auto numZeros = std::count(yaexistentes.cbegin(), yaexistentes.cend(), 0);
   numeros posibles(numZeros);

   digit j = 0;
   for(digit k = 0; k < lado; ++k)
   {
      if(0 == yaexistentes[k])
      {
         posibles[j] = k + 1;
         ++j;
      }
   }

   //Print(posibles);

   return posibles;
}


ESolucion SolucionarCelda(sudoku & s);
ESolucion SolucionarGrupoCeldas(const aDigit9 &indices, sudoku & s);


ESolucion Solucionar(sudoku &s)
{
   auto bContinuar = true;
   while(bContinuar)
   {
      bContinuar = false;

      const ESolucion eRet = SolucionarCelda(s);
      if(ESolucion::Ok == eRet || ESolucion::Imposible == eRet)
      {
         return eRet;
      }
      else if(ESolucion::Avanzo == eRet)
      {
         bContinuar = true;
      }
      // Si no he relleando ninguna celda, busco si en un bloque un nº sólo es posible en una celda.
      // Es decir, busco en todas las celdas libres de un bloque, y acumulo los posibles, si hay algún
      // posibles sin repetir es que sólo se pueden en una posición.
      else if(ESolucion::No_avanzo == eRet)
      {
         for(digit fila = 0; fila < lado; ++fila)
         {
            const auto indices = ObtenerIndicesFila(fila);
            auto eRetGrupo = SolucionarGrupoCeldas(indices, s);
            if(ESolucion::Avanzo == eRetGrupo)
               bContinuar = true;
         }


         for(digit col = 0; col < lado; ++col)
         {
            const auto indices = ObtenerIndicesColumna(col);
            auto eRetGrupo = SolucionarGrupoCeldas(indices, s);
            if(ESolucion::Avanzo == eRetGrupo)
               bContinuar = true;
         }


         for(digit bloque = 0; bloque < lado; ++bloque)
         {
            const auto indices = ObtenerIndicesBloque(bloque);
            auto eRetGrupo = SolucionarGrupoCeldas(indices, s);
            if(ESolucion::Avanzo == eRetGrupo)
               bContinuar = true;
         }
      }
   }

   auto numCeros = std::count(s.cbegin(), s.cend(), hole);
   //std::cout << numCeros << " celdas pendientes" << '\n';
   //PrintSudoku(s);

   //bool solucionado = (std::find(s.cbegin(), s.cend(), hole) == s.cend());
   //return solucionado ? ESolucion::Ok : ESolucion::No_avanzo;
   return (numCeros == 0) ? ESolucion::Ok : ESolucion::No_avanzo;
}


ESolucion SolucionarCelda(sudoku & s)
{
   // Si ninguna celda del sudoku es un "hole", ya he solucionado el sudoku
   ESolucion eRet = ESolucion::Ok;

   for(digit i = 0; i < lado2; ++i)
   {
      if(hole == s[i])
      {
         const numeros posibles = NumerosPosibles(s, i);
         const auto numPosibles = posibles.size();

         if(numPosibles > 1)
         {
            if(ESolucion::Ok == eRet)
               eRet = ESolucion::No_avanzo;
         }
         else if(1 == numPosibles)
         {
            //std::cout << "Avanzo por celda" << '\n';
            UpdateSudoku(s, i, posibles.front(), true);
            //std::cout << "Fila:" << f << ", Columna:" << c << ", Bloque:" << bloque << " => " << s[i] << '\n';

            eRet = ESolucion::Avanzo; // Dar otra vuelta al sudoku porque hemos resuelto alguna celda en esta iteración
         }
         else //(0 == numPosibles)
         {
            //std::cout << "Abandono este sudoku" << '\n';
            //PrintSudoku(s, i);
            return ESolucion::Imposible; // Es imposible encontrar una solución por esta vía
         }
      }
   }

   return eRet;
}


ESolucion SolucionarGrupoCeldas(const aDigit9 &indices, sudoku & s)
{
   ESolucion eRet = ESolucion::No_avanzo;

   // Las repeticiones del nº 1 se guarda en el elemento [0] del array. Las del nº 2 en el [1] ...
   aDigit9 repeticiones = { 0 };
   // El índice de la última celda donde el nº1 es posible se guarda en el elemento [0] del array.
   aDigit9 valueToIndex = { 0 };

   for(const auto i : indices)
   {
      if(hole == s[i])
      {
         const numeros posibles = NumerosPosibles(s, i);
         for(auto posible : posibles)
         {
            ++repeticiones[posible - 1]; // Las repeticiones del nº 1 se guarda en el elemento [0] del array. Las del nº 2 en el [1] ...
            valueToIndex[posible - 1] = i;
         }
      }
   }

   for(digit j = 0; j < lado; ++j)
   {
      if(1 == repeticiones[j])
      {
         const digit solution = j + 1;
         //std::cout << "Avanzo grupo" << '\n';
         UpdateSudoku(s, valueToIndex[j], solution, true);

         eRet = ESolucion::Avanzo;
      }
   }

   return eRet;
}




ESolucion SolucionarFuerzaBruta(sudoku &s)
{
   for(digit i = 0; i < lado2; ++i)
   {
      // Si la celda no ha sido completada
      if(0 == s[i])
      {
         const auto posibles = NumerosPosibles(s, i);
         const auto numPosibles = posibles.size();
         if(0 == numPosibles)
         {
            //std::cout << "Abandono esta copia i=" << i << '\n';
            //PrintSudoku(s, i);
            return ESolucion::Imposible; // Es imposible encontrar una solución por esta vía
         }
         else if(1 == numPosibles)
         {
            UpdateSudoku(s, i, posibles.front());
         }
         else // Caso general
         //else if(2 == numPosibles) // Caso web leetcode
         {
            digit count = 1;
            for(auto posible : posibles)
            {
               //std::cout << "Copiando sudoku i=" << i  << " (" << count << "/" << numPosibles << ")" << '\n';
               sudoku copia(s);
               copia[i] = posible;
               //PrintSudoku(copia, i);

               auto eSol1 = Solucionar(copia);
               if(ESolucion::Ok == eSol1)
               {
                  s = copia;
                  return eSol1;
               }
               else if(ESolucion::No_avanzo == eSol1)
               {
                  auto eSol2 = SolucionarFuerzaBruta(copia);
                  if(ESolucion::Ok == eSol2)
                  {
                     s = copia;
                     return eSol2;
                  }
               }

               ++count;
            }

            return ESolucion::Imposible;
         }
      }
   }

   return ESolucion::Ok;
}

int main()
{
   auto start = std::chrono::steady_clock::now();


   // Leet code
   sudoku s =
   {
      0, 0, 9, 7, 4, 8, 0, 0, 0,
      7, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 2, 0, 1, 0, 9, 0, 0, 0,
      0, 0, 7, 0, 0, 0, 2, 4, 0,
      0, 6, 4, 0, 1, 0, 5, 9, 0,
      0, 9, 8, 0, 0, 0, 3, 0, 0,
      0, 0, 0, 8, 0, 3, 0, 2, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 6,
      0, 0, 0, 2, 7, 5, 9, 0, 0,
   };

   /*sudoku s =
   {
      5, 3, 0, 0, 7, 0, 0, 0, 0,
      6, 0, 0, 1, 9, 5, 0, 0, 0,
      0, 9, 8, 0, 0, 0, 0, 6, 0,
      8, 0, 0, 0, 6, 0, 0, 0, 3,
      4, 0, 0, 8, 0, 3, 0, 0, 1,
      7, 0, 0, 0, 2, 0, 0, 0, 6,
      0, 6, 0, 0, 0, 0, 2, 8, 0,
      0, 0, 0, 4, 1, 9, 0, 0, 5,
      0, 0, 0, 0, 8, 0, 0, 7, 9
   };*/

   // Difícil
   //sudoku s = { 0 };

   // Difícil
   /*sudoku s =
   { 2, 0, 0, 3, 0, 0, 5, 1, 0,
     4, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 6, 0, 5, 9, 0, 4, 0, 0,
     0, 0, 5, 1, 0, 0, 0, 0, 0,
     0, 8, 7, 0, 4 ,0, 0, 3, 0,
     0, 0, 0, 6, 0, 0, 9, 0, 2,
     0, 0, 0, 0, 1, 0, 0, 0, 0,
     7, 3, 0, 4, 0, 0, 0, 5, 0,
     0, 0, 8, 0, 0, 7, 0, 0, 0 };*/

     // Fácil
     /*sudoku s =
     { 0, 3, 0, 2, 6, 7, 5, 0, 0,
       0, 0, 9, 3, 4, 0, 7, 0, 6,
       0, 0, 7, 5, 0, 0, 0, 4, 0,
       0, 0, 5, 6, 0, 0, 4, 3, 7,
       7, 6, 0, 1, 0, 4, 0, 2, 5,
       2, 0, 3, 0, 0, 5, 0, 6, 8,
       4, 0, 0, 0, 5, 3, 0, 7, 2,
       0, 0, 0, 9, 2, 0, 3, 0, 4,
       3, 5, 2, 4, 0, 6, 8, 9, 0 };*/


       /* sudoku s =
        { 0, 3, 0, 2, 6, 7, 5, 0, 0,
          0, 0, 9, 3, 4, 0, 7, 0, 6,
          0, 0, 7, 5, 0, 0, 0, 4, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0,
          4, 0, 0, 0, 0, 0, 0, 0, 2,
          0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0 };
   */

   PrintSudoku(s);

   ESolucion eSol;
   eSol = Solucionar(s);
   //// Hay que intentarlo por fuerza bruta
   if(ESolucion::No_avanzo == eSol)
   {
      PrintSudoku(s);
      std::cout << "Lo intento por fuerza bruta" << '\n';

      //for(int i = 1; i <= 9; ++i)
      //{
      //   std::cout << i << ": " << std::count(s.cbegin(), s.cend(), i) << " veces" << '\n';
      //}

      //// Las repeticiones del nº 1 se guarda en el elemento [0] del array. Las del nº 2 en el [1] ...
      //aDigit9 repeticiones = { 0 };
      //// El índice de la última celda donde el nº1 es posible se guarda en el elemento [0] del array.
      //aDigit9 valueToIndex = { 0 };
      //aVecDigit81 sudokuPosibles;

      //for(int i=0; i < lado2; ++i)
      //{
      //   if(hole == s[i])
      //   {
      //      const numeros posibles = NumerosPosibles(s, i);
      //      sudokuPosibles[i] = posibles;
      //      for(auto posible : posibles)
      //      {
      //         ++repeticiones[posible - 1]; // Las repeticiones del nº 1 se guarda en el elemento [0] del array. Las del nº 2 en el [1] ...
      //         valueToIndex[posible - 1] = i;
      //      }
      //   }
      //}


      eSol = SolucionarFuerzaBruta(s);
      if(ESolucion::Imposible == eSol)
      {
         std::cout << "ERROR: no he podido solucionar este sudoku" << '\n';
      }
   }

   if(ESolucion::Ok == eSol)
   {
      std::cout << "Solucionado:" << '\n';
      PrintSudoku(s);
   }


   // do something
   auto finish = std::chrono::steady_clock::now();
   auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
   std::cout << elapsed.count() << " ms";


   system("pause");
}


