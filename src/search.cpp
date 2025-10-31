#include "search.hpp"

#include "move_list.hpp"

static Score Quiescence(Position& pos, TranspositionTable& table, int depth, Score alpha, Score beta) {

}

static Score Negamax(Position& pos, TranspositionTable& table, int depth, Score alpha, Score beta) {
    if (depth <= 0) return Quiescence(pos, table, depth, alpha, beta);
    /*
    Move bestMove = Move::NewNone();
    TranspositionTable::Entry tableEntry = table.GetEntry(pos.GetZobristHash());
    if (tableEntry.IsValid()) {
        bestMove = tableEntry.GetBestMove();
        Score score = tableEntry.GetScore();
        if (tableEntry.GetType() == TranspositionTable::Entry::Type::PV) return score;
        if (tableEntry.GetType() == TranspositionTable::Entry::Type::Fail_Low && score > alpha) alpha = score;
        if (tableEntry.GetType() == TranspositionTable::Entry::Type::Fail_High && score < beta) beta = score;        
        if (beta <= alpha)
    } 
    */
    
    MoveList moves(pos);
    Move bestMove = Move::NewNone();
    for (Move move : moves) {
        pos.DoMove(move);
        Score score = -Negamax(pos, table, depth - 1, -beta, -alpha);
        pos.UndoMove();
        if (score > alpha) {
            alpha = score;
            bestMove = move;
            if (alpha > beta) {
                return alpha;
            }
        }
    }
    table.SetEntry(TranspositionTable::Entry(
        pos.GetZobristHash(), bestMove, alpha, depth, TranspositionTable::Entry::Type::PV
    ));
    return alpha;
}

void Search(Position& pos, TranspositionTable& table, int depth) {
    Negamax(pos, table, depth, SCORE_MIN, SCORE_MAX);
}
