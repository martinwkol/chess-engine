#include "search.hpp"
#include "evaluate.hpp"
#include "move_list.hpp"

static Score Quiescence(Position& pos, Score alpha, Score beta) {
    Score bestScore = SCORE_MIN;
    if (!pos.IsCheck()) {
        // Stand pat
        bestScore = Evaluate(pos);
        if (bestScore >= beta) return bestScore;
        if (bestScore > alpha) alpha = bestScore;
    }

    MoveList moves(pos);
    for (Move move : moves) {
        // TODO: Search for moves that check opponent
        // TODO: Static exchange evaluation 
        if (!pos.IsCheck() && !move.IsCapture()) continue;
        pos.DoMove(move);
        Score score = -Quiescence(pos, -beta, -alpha);
        pos.UndoMove();

        if (score >= beta) { // Fail high
            return score;
        }
        if (score > bestScore) {
            bestScore = score;
            if (score > alpha) {
                alpha = score;
            }
        }
    }

    return bestScore;
}

static Score Negamax(Position& pos, TranspositionTable& table, int depth, Score alpha, Score beta) {
    if (depth <= 0) return Quiescence(pos, alpha, beta);
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
    Score bestScore = SCORE_MIN;
    Move bestMove = Move::NewNone();
    for (Move move : moves) {
        pos.DoMove(move);
        Score score = -Negamax(pos, table, depth - 1, -beta, -alpha);
        pos.UndoMove();
        if (score >= beta) { // Fail high
            table.SetEntry(TranspositionTable::Entry(
                pos.GetZobristHash(), move, score, depth, TranspositionTable::Entry::Type::Fail_High
            ));
            return score;
        }
        if (score > bestScore) {
            bestScore = score;
            if (score > alpha) {
                alpha = score;
            }
        }
    }

    // Fail low?
    TranspositionTable::Entry::Type entryType = alpha <= bestScore ? TranspositionTable::Entry::Type::PV : TranspositionTable::Entry::Type::Fail_Low;

    table.SetEntry(TranspositionTable::Entry(
        pos.GetZobristHash(), bestMove, bestScore, depth, entryType
    ));
    return bestScore;
}

void Search(Position& pos, TranspositionTable& table, int depth) {
    Negamax(pos, table, depth, SCORE_MIN, SCORE_MAX);
}
